#include <cassert>
#include <unordered_map>

#include "common.h"
#include "Expression.h"




namespace {


	inline OpCode i(Mnemonic m, AddressMode mode) {
		return OpCode(Instruction(m65816, m), mode);
	}

	bool inrange(ExpressionPtr e, uint32_t pc, const identifier_map &map) {

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




}


void assign_pc(BasicLine *line, uint32_t &pc, identifier_map *map = nullptr) {

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
			case START:
			case END:
			case DATA:
			case EXPORT:
			case PRAGMA:
			case ALIGN:
				// should never happen.
				assert("unexpected directive!");
				return;

			case PROLOGUE: 
			case EPILOGUE:
				// todo -- when does this code generate?
				break;

			case DCB:
				assert(operand->is_vector());
				pc += ((VectorExpression *)operand)->size() * 1;
				return;

			case DCW:
				assert(operand->is_vector());
				pc += ((VectorExpression *)operand)->size() * 2;
				return;

			case DCL:
				assert(operand->is_vector());
				pc += ((VectorExpression *)operand)->size() * 4;
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

		if (op.isRelative() && line->long_branch) {
			/*
			 *
			 * long branches.
			 * bcc foo      [2 bytes]
			 * becomes
			 * bcs *+5      [2 bytes
			 * brl foo      [3 bytes]
			 */
			if (op.mnemonic() == BRA) pc += 3;
			else pc += 5;
			return;
		}
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
	}
}

// does it also need to set the segment name to 0 in the map?
void fix_branches(BlockQueue &blocks) {

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

		for (auto block : blocks) {
			block->pc = pc;
			if (block->label) map.emplace(std::make_pair(block->label, pc));
			for (auto line : block->lines) {
				assign_pc(line, pc, &map);
			}
		}

		// 2. if a relative branch is out of range, make it long.

		uint32_t pc_fudge = 0;
		for (auto block : blocks) {

			// only need to check last
			for (auto line : block->lines) {
				OpCode op = line->opcode;
				if (!op) continue;
				if (op.addressMode() != relative) continue;

				// check if in range....
				uint32_t pc = line->pc + pc_fudge;
				if (inrange(line->operands[0], pc, map)) continue;

				//
				delta = true;

				uint32_t fudge;
				if (op.mnemonic() == BRA) {
					fudge += 1;
					line->opcode = i(BRL, relative_long);
				} else {
					fudge += 3;
					line->long_branch = true;
				}

				// adjust all future labels.
				for (auto &kv : map) { if (kv.second >= pc) kv.second += fudge; }
				pc_fudge += fudge;
			}
		}


	} while (delta);

}

