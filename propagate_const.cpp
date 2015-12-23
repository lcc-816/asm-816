

#include <unordered_set>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"
#include "OpCode.h"
#include "dp_register.h"


/*
 * const propagation.
 *
 *
 * keep it simple for now -- stz only...
 * stz %t0
 * ...
 * pei %t0 -> pea #0
 * lda %t0 -> lda #0
 * adc %t0 -> adc #0
 * etc.
 *
 */


// returns true if any changes made.
bool propagate_const(LineQueue &list) {

	LineQueue optimized;
	bool delta = false;

	dp_register reg;

	std::unordered_set<dp_register> known_zeroes;

	for (auto iter = list.begin(); iter != list.end(); ++iter){
		auto line = *iter;

		OpCode opcode = line->opcode;
		AddressMode mode = opcode.addressMode();
		ExpressionPtr e = line->operands[0];

		switch (opcode.mnemonic()) {
			case STZ:
				if (opcode.addressMode() == zp && e->is_temporary(reg)) {
					known_zeroes.insert(reg);
				}
				break;

#if 0
			case STA:
			case STX:
			case STY:
				// rmw
			case TSB:
			case TRB:
			case INC:
			case DEC:
			case ASL:
			case LSR:
			case ROR:
			case ROL:
#endif




			case PEI:
				if (e->is_temporary(reg)) {
					if (known_zeroes.find(reg) != known_zeroes.end()) {
						line->opcode = OpCode(m65816, PEA, absolute);
						line->operands[0] = Expression::Integer(0);
						line->calc_registers();
						delta = true;
					}
				}
				break;

			case LDA:
			case LDX:
			case LDY:
			case ADC:
			case SBC:
			case EOR:
			case AND:
			case ORA:
				if (opcode.addressMode() == zp && e->is_temporary(reg)) {
					if (known_zeroes.find(reg) != known_zeroes.end()) {
						line->opcode = OpCode(m65816, opcode.mnemonic(), immediate);
						line->operands[0] = Expression::Integer(0);
						line->calc_registers();
						delta = true;
					}	
				}
				break;

			default:

				if (opcode.writes_zp() && opcode.addressMode() == zp && e->is_temporary(reg)) {
					known_zeroes.erase(reg);
				}
				break;
		}

	}

	return delta;
}