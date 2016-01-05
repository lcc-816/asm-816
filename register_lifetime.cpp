#include "types.h"
#include "common.h"
#include "register_set.h"
#include "Expression.h"


/*
 * checks for dead-writes to wdc registers (a, x, y, etc.)
 *
 *
 */


/*

lda, etc -- add a as a dependency when short m?

lda #0
sep #$30
lda %p0
rep #$30
*/


bool register_lifetime(BasicBlockPtr block) {

	// 	eliminate dead writes for real registers.

	register_set live = register_set::register_mask(); // assume all registers needed for the next block.

	LineQueue out;

	std::copy_if(block->lines.rbegin(), block->lines.rend(), std::front_inserter(out), [&live](BasicLinePtr &line){

		OpCode opcode = line->opcode;
		if (!opcode) return true;

		register_set rs = line->read_registers();
		register_set ws = line->write_registers();

		register_set olive = live;

		switch(opcode.mnemonic()) {
			case JML:
			case JMP:
			case JSL:
			case JSR:
			case RTI:
			case RTL:
			case RTS:
				// a, x, y could be passed as parameters.
				rs += register_set(0x03); // magic!
				break;
			case REP:
			case SEP:
				// try to limit the bits changed!
				{
					uint32_t p;
					if (line->operands[0] && line->operands[0]->is_integer(p)) {
						ws = register_set(p << 8);
					}
					// todo -- if going to short m, add a as a dependency?
				}
				break;
			default:
				break;
		}


		live -= ws;
		live += rs;
		live &= register_set::register_mask();

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

		//register_set wx = ws - rs;  // registers written but not read.
		/*
		 r:  a - - a 
		 w:  - a - a
		 wx: - a - -

		 */
		//wx &= register_set::register_mask();

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