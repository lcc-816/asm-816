
#include "Machine.h"
#include "dp_register_set.h"
#include "common.h"
#include "Expression.h"

namespace {
#include "gen-attr-table.h"
}

namespace {

	unsigned classify(OpCode op, bool longM, bool longX, unsigned &bytes) {

		AddressMode mode = op.addressMode();

		switch (mode)
		{
			// lda [%t0] lda [%t0],y
		case zp_indirect_long:
		case zp_indirect_long_y:
			bytes = 3;
			return reg_read;	// reg_read_indirect?

			// lda (%t0)
		case zp_indirect:
		case zp_indirect_y:
			bytes = 2;
			return reg_read;

		default:
			break;
		}

		switch (op.mnemonic()) {
			case ASL:
			case DEC:
			case INC:
			case LSR:
			case ROL:
			case ROR:
			case TRB:
			case TSB:
				bytes = longM ? 2 : 1;
				return reg_rw;

			case ADC:
			case AND:
			case BIT:
			case CMP:
			case EOR:
			case LDA:
			case ORA:
			case SBC:
				bytes = longM ? 2 : 1;
				return reg_read;

			case CPX:
			case CPY:
			case LDX:
			case LDY:
				bytes = longX ? 2 : 1;
				return reg_read;

			case PEI: // may be caught above by zp_indirect.
				bytes = 2;
				return reg_read;


			// write.

			case STA:
			case STZ:
				bytes = longM ? 2 : 1;
				return reg_write;
			case STX:
			case STY:
				bytes = longX ? 2 : 1;
				return reg_write;

			default:
				return reg_none;
		}
	}
}

register_set BasicLine::read_registers() const {

	if (opcode) return read_table[opcode.opcode()];
	if (directive == SMART_BRANCH) return branch.read_registers();
	return register_set();
}

register_set BasicLine::write_registers() const {

	if (opcode) return write_table[opcode.opcode()];
	if (directive == SMART_BRANCH) return branch.write_registers();
	return register_set();
}


void BasicLine::calc_registers() {

	// update reg, reg_count, and reg_status.

	reg_status = reg_none;
	reg_count = 0;
	reg = dp_register();

	if (!opcode) {
		return;
	}

	const auto &e = operands[0];
	// in the future, may want to track %p variables?
	if (e && e->is_register() && e->is_register(reg)) {

		unsigned bytes;
		reg_status = classify(opcode, longM, longX, bytes);

		unsigned a = reg.number & ~0x01;
		unsigned b = reg.number + bytes + 1; // +1 to round up for division below.

		reg_count = (b - a) >> 1;
		reg.number &= ~0x01;
		return;
	}

}