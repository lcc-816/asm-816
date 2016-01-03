#include "types.h"
#include "common.h"
#include "register_bits.h"
#include "Expression.h"

#include "attr-table.h"

/*
 * checks for dead-writes to wdc registers (a, x, y, etc.)
 *
 *
 */

register_bits read_registers(BasicLinePtr line) {
	if (line->opcode) return read_table[line->opcode.opcode()];
	// smart branches?
	return register_bits();
}

register_bits write_registers(BasicLinePtr line) {
	if (line->opcode) return write_table[line->opcode.opcode()];
	// smart branches?
	return register_bits();
}


bool register_lifetime(BasicBlockPtr block) {

	// 	eliminate dead writes for real registers.

	register_bits live = register_bits::register_mask(); // assume all registers needed for the next block.

	LineQueue out;

	std::copy_if(block->lines.rbegin(), block->lines.rend(), std::front_inserter(out), [&live](BasicLinePtr &line){

		OpCode opcode = line->opcode;
		if (!opcode) return true;

		register_bits rs = read_registers(line);
		register_bits ws = write_registers(line);

		register_bits olive = live;

		switch(opcode.mnemonic()) {
			case JML:
			case JMP:
			case JSL:
			case JSR:
			case RTI:
			case RTL:
			case RTS:
				// a, x, y could be passed as parameters.
				rs += register_bits(0x03); // magic!
				break;
			case REP:
			case SEP:
				// try to limit the bits changed!
				{
					uint32_t p;
					if (line->operands[0] && line->operands[0]->is_integer(p)) {
						ws = register_bits(p << 8);
					}
				}
				break;
			default:
				break;
		}


		live -= ws;
		live += rs;
		live &= register_bits::register_mask();

		// protected instructions. 
		// branches not included since they end they basic block and are stored elsewhere.
		switch(opcode.mnemonic()) {
			case BRK:
			case COP:
			case JML:
			case JMP:
			case JSL:
			case JSR:
			case MVN: //
			case MVP: //
			case NOP:
			case RTI:
			case RTL:
			case RTS:
			case STP:
			case WAI:
			case WDM:
			case XCE:
			case SEI:
			case CLI:
				return true;
			default:
				break;
		}

		// if it reads memory, don't kill it.
		if (rs.memory()) return true;
		// if it writes memory or the stack don't kill it.
		if (ws.memory() || ws.zp() || ws.stack() || ws.s()) return true;

		//register_bits wx = ws - rs;  // registers written but not read.
		/*
		 r:  a - - a 
		 w:  - a - a
		 wx: - a - -

		 */
		//wx &= register_bits::register_mask();

		// if it doesn't write to any live registers, kill it!
		if (!(ws & olive)) {
			live = olive;
			return false;
		}
		return true;
	});

	if (out.size() != block->lines.size()) {
		block->lines = std::move(out);
		return true;
	}
	return false;
}