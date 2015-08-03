
#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"
#include "OpCode.h"

#include <experimental/optional>

using namespace std::experimental;


optional<uint16_t> &operator += (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs + rhs);
	return lhs;
}

optional<uint16_t> &operator -= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs - rhs);
	return lhs;
}

optional<uint16_t> &operator |= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs | rhs);
	return lhs;
}

optional<uint16_t> &operator &= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs & rhs);
	return lhs;
}

optional<uint16_t> &operator ^= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs ^ rhs);
	return lhs;
}

bool reg_const(LineQueue &list) {



	optional<uint16_t> reg_a;
	optional<uint16_t> reg_x;
	optional<uint16_t> reg_y;

	bool delta = false;

	LineQueue out;

	while (!list.empty()) {
		bool kill = false;

		BasicLine *line = list.front();
		list.pop_front();


		ExpressionPtr e = line->operands[0];
		const OpCode &op = line->opcode;

		if (line->directive == SMART_BRANCH && line->branch.writes_a()) {
			// branch could clobber a ... but it will also be at the end of the block,
			reg_a = nullopt;
		}
		if (op) {

			uint32_t value;
			optional<uint16_t> tmp;

			switch(op.mnemonic()) {

			case LDA:
				std::swap(tmp, reg_a); // resets a to unknown...

				if (op.addressMode() == immediate && e->is_integer(value)) {

					value &= 0xffff;
					if (tmp == (uint16_t)value)
						kill = true;
					
					reg_a = value;
				}
				break;

			case LDX:
				std::swap(tmp, reg_x); // resets a to unknown...

				if (op.addressMode() == immediate && e->is_integer(value)) {

					value &= 0xffff;
					if (tmp == (uint16_t)value)
						kill = true;
					
					reg_x = value;
				}
				break;

			case LDY:
				std::swap(tmp, reg_y); // resets a to unknown...

				if (op.addressMode() == immediate && e->is_integer(value)) {

					value &= 0xffff;
					if (tmp == (uint16_t)value)
						kill = true;
					
					// todo -- if w/in 1, change to INY/DEY...

					reg_y = value;
				}
				break;


			case DEC:
				if (op.addressMode() == implied) {

					reg_a -= 1;
				}
				break;

			case INC:
				if (op.addressMode() == implied) {

					reg_a += 1;
				}
				break;

			case DEX:
				reg_x -= 1;
				break;

			case INX:
				reg_x += 1;
				break;

			case DEY:
				reg_y -= 1;
				break;

			case INY:
				reg_y += 1;
				break;

			case TAY:
				reg_y = reg_a;
				break;
			case TAX:
				reg_x = reg_a;
				break;
			case TXA:
				reg_a = reg_x;
				break;
			case TYA:
				reg_a = reg_y;
				break;

			// asl, lsr, adc, etc... but they are affected by p.c


			case AND:
				if (op.addressMode() == immediate && e->is_integer(value)) {

					value &= 0xffff;
					reg_a &= value;
				}
				else reg_a = nullopt;
				break;

			case EOR:
				if (op.addressMode() == immediate && e->is_integer(value)) {

					value &= 0xffff;
					reg_a ^= value;
				}
				else reg_a = nullopt;
				break;

			case ORA:
				if (op.addressMode() == immediate && e->is_integer(value)) {

					value &= 0xffff;
					reg_a |= value;
				}
				else reg_a = nullopt;
				break;


			default:
				if (op.writes_a()) reg_a = nullopt;
				if (op.writes_x()) reg_x = nullopt;
				if (op.writes_y()) reg_y = nullopt;			
				break;

			}


		}



		if (kill) {
			delta = true;
			delete line;
		}
		else {
			out.push_back(line);
		}

	}

	list = std::move(out);

	return delta;
}