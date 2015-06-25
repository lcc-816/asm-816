// assume dp address mode.  These are for temporary register lifetime analysis.


#include "Machine.h"
#include "OpCode.h"
#include "common.h"


bool instruction_rmw(Mnemonic m) {
	switch(m)
	{
		case ASL:
		case DEC:
		case INC:
		case LSR:
		case ROL:
		case ROR:
		case TRB:
		case TSB:
			return true;

		default:
			return false;
	}
}


bool instruction_read(Mnemonic m)
{
	switch(m)
	{
	case ADC:
	case AND:
	case BIT:
	case CMP:
	case CPX:
	case CPY:
	case EOR:
	case LDA:
	case LDX:
	case LDY:
	case ORA:
	case SBC:

	case PEI:

		return true;
	default:
		return instruction_rmw(m);
	}
}


bool instruction_write(Mnemonic m) {
	switch(m)
	{
		case STA:
		case STX:
		case STY:
		case STZ:
			return true;
		default:
			return instruction_rmw(m);
	}
}

// todo -- database for all instructions/modes -- reads_a/x/y/, writes_a/x/y/s/etc, memory, individual flags


// todo -- STA [dp] is actually a (long) read.  Need to take an opcode and check the mode.

#if 0
unsigned classify(Mnemonic m) {
	if (instruction_rmw(m)) return reg_rw;
	if (instruction_read(m)) return reg_read;
	if (instruction_write(m)) return reg_write;

	return reg_none;
}

unsigned classify(OpCode op) {

	AddressMode mode = op.addressMode();

	// [dp] and [dp],y are always long reads, from this perspective.

	if (mode == zp_indirect_long || mode == zp_indirect_long_y)
		return reg_read_long;

	switch (op.mnemonic()) {
		case ASL:
		case DEC:
		case INC:
		case LSR:
		case ROL:
		case ROR:
		case TRB:
		case TSB:
			return reg_rw;

		case ADC:
		case AND:
		case BIT:
		case CMP:
		case CPX:
		case CPY:
		case EOR:
		case LDA:
		case LDX:
		case LDY:
		case ORA:
		case SBC:

		case PEI:
			return reg_read;


		case STA:
		case STX:
		case STY:
		case STZ:
			// write.
			return reg_write;

		default:
			return reg_none;
	}
}
#endif
