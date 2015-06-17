#include <cassert>

#include "common.h"
#include "Expression.h"

void set_pc(BasicLine *line, uint32_t &pc) {

	line->pc = pc;

	if (line->label) {
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
			case PROLOGUE:
			case EPILOGUE:
			case ALIGN:
				// should never happen.
				assert("unexpected directive!");
				return;

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
				assert(operand->is_integer(x));
				pc += x;
				return;
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


void set_pc(LineQueue &lines) {
	uint32_t pc = 0;

	for (auto line : lines) {
		set_pc(line, pc);
	}	
}


void set_pc(BlockQueue &blocks) {
	uint32_t pc = 0;

	for (auto block : blocks) {

		for (auto line : block->lines) {
			set_pc(line, pc);
		}
	}
}