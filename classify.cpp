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


unsigned classify(Mnemonic m) {
	if (instruction_rmw(m)) return reg_rw;
	if (instruction_read(m)) return reg_read;
	if (instruction_write(m)) return reg_write;

	return reg_none;
 }
