#include <cassert>
#include <unordered_map>

#include "common.h"
#include "Expression.h"
#include "branch.h"

/*
 * assign a pc to each line.  This also converts local labels to relative form.
 * fix_branches -- extend out-of range branches. (bra -> brl)
 *
 */


namespace {


	inline OpCode i(Mnemonic m, AddressMode mode) {
		return OpCode(Instruction(m65816, m), mode);
	}

	bool in_range(ExpressionPtr e, uint32_t pc, const identifier_map &map) {

		try {

			uint32_t target = e->evaluate(pc, map);
			// pc is pc before the instruction.  actual pc is + 2.
			pc += 2;
			if (target > pc + 0x7f) return false;
			if (target < pc - 0x80) return false;

			return true;

		} catch (std::exception &ex) {
			return false;
		}
	}


	// returns true if in-range.
	bool in_range(ExpressionPtr e, uint32_t pc, const identifier_map &map, const branch &b) {

		if (b.far) return true;

		try {

			uint32_t target = e->evaluate(pc, map);
			if (b.in_range(pc, target)) return true;
			return false;

		} catch (std::exception &ex) {
			std::string s = e->to_string();
			fprintf(stderr, "Unable to evaluate smart branch target: %s.  Assuming far branch.\n", s.c_str());
			return false;
		}
	}


}


void assign_pc(BasicLinePtr line, uint32_t &pc, identifier_map *map = nullptr) {

	if (!line) return;

	line->pc = pc;

	if (line->label) {
		if (map)
			map->emplace(std::make_pair(line->label, pc));
		return;
	}

	if (line->directive != kUndefinedDirective) {
		ExpressionPtr operand = line->operands[0];

		switch (line->directive) {
			case kUndefinedDirective:
			case ALIGN:
				// should never happen.
				assert("unexpected directive!");
				return;

			case DCB:
				assert(operand->is_vector());
				pc += std::static_pointer_cast<VectorExpression>(operand)->size() * 1;
				return;

			case DCW:
				assert(operand->is_vector());
				pc += std::static_pointer_cast<VectorExpression>(operand)->size() * 2;
				return;

			case DCL:
				assert(operand->is_vector());
				pc += std::static_pointer_cast<VectorExpression>(operand)->size() * 4;
				return;

			case SMART_BRANCH:
				pc += line->branch.size();
				return;

			case DS:
				// todo -- simplify stuff like
				// ds 512-*
				//
				uint32_t x;
				if (operand->is_integer(x)) {
					pc += x;
					return;
				}
				throw std::runtime_error("Expression is not absolute");
		}
	}

	if (line->opcode) {

		OpCode op = line->opcode;
		pc += op.bytes(line->longM, line->longX);
	}
}


void assign_pc(LineQueue &lines) {
	uint32_t pc = 0;

	identifier_map map;

	for (auto line : lines) {
		assign_pc(line, pc, &map);
	}

	// convert to relative labels.
	for (auto line : lines) {

		for (auto &e : line->operands) {
			if (e) e = e->make_relative(line->pc, map);
		}

	}
}


void assign_pc(BlockQueue &blocks) {
	uint32_t pc = 0;

	for (auto block : blocks) {
		block->pc = pc;
		for (auto line : block->lines) {
			assign_pc(line, pc);
		}
		assign_pc(block->exit_branch, pc);
	}

}

void fix_branches(Segment *segment, BlockQueue &blocks) {

	// rewrite short branches, if necessary.
	// todo -- optimization
	// multi-branches could be optimized
	// eg
	// if (a <= b) goto foo;
	// bcc foo
	// beq foo
	//
	// expands to
	// bcs *+5
	// bra foo
	// bne *+5
	// bra foo

	bool delta;

	do {
		delta = false;

		// 1. assign a pc to all lines

		uint32_t pc = 0;
		identifier_map map;

		// insert function name 
		// this is not actually correct... need to insert as a label so
		// optimizer can work correctly.
		if (segment && segment->name) {
			map.emplace(std::make_pair(segment->name, pc));
		}

		for (auto block : blocks) {
			block->pc = pc;
			if (block->label) map.emplace(std::make_pair(block->label, pc));
			for (auto line : block->lines) {
				assign_pc(line, pc, &map);
			}
			assign_pc(block->exit_branch, pc, &map);
		}

		// 2. if a relative branch is out of range, make it long.

		uint32_t pc_fudge = 0;
		for (auto block : blocks) {

			auto line = block->exit_branch;
			if (line && line->directive == SMART_BRANCH) {

				uint32_t fudge = 0;
				uint32_t pc = line->pc + pc_fudge;
				if (in_range(line->operands[0], pc, map, line->branch)) continue;
				// if it's already a far branch... oops!
				
				delta = true;
				line->branch.far = true;
				fudge = line->branch.make_far();

				// adjust all future labels.
				for (auto &kv : map) { if (kv.second >= pc) kv.second += fudge; }
				pc_fudge += fudge;
			}
		}


	} while (delta);

	// check if regular branches are in-range, too?

}

