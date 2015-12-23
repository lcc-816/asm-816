#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Expression.h"
#include "register_set.h"

#include "common.h"
#include "Machine.h"



/*
 * assign a dp register location for all %t, %r, %v, %p dp_registers. 
 * This also generates epilogue / prologue code.
 *
 */

namespace {
	ExpressionPtr star_plus(unsigned offset) {
		// generate an expression ptr for *+offset
		return Expression::Binary('+', Expression::PC(), Expression::Integer(offset));
	}
}


/*
 * convert dp registers to stack relative...
 *
 * 1. check for adc #%dp -- not allowed (must be done above, before reg assignment)
 * 2. check for zp,x zp,y (zp), (zp,x), [zp] [zp],y -- not allowed
 * 3. check if opcode has stack_relative as a possible address mode
 *
 * then
 *
 * 1. swap address mode / operand.
 */

bool can_convert_to_sr(const BlockQueue &blocks) {


	for (auto block : blocks) {

		for (auto line : block->lines) {

			OpCode op = line->opcode;
			ExpressionPtr e = line->operands[0];

			// hmm... how can we track it... consider stdcall vs cdecl...
			switch (op.mnemonic()) {
				// todo -- use attributes...
				case PEI:
				case PER:
				case PEA:
				case PHB:
				case PHK:
				case PHA:
				case PHX:
				case PHY:
				case TCS:
				case TSC:
				case PLA:
				case PLX:
				case PLY:
				case PLB:
				case PHP:
				case PLP:
				case PHD:
				case PLD:
				case TDC: // could replace w/ TSC?
				case TCD:
					return false;
				default:
					break;
			}

			switch (op.addressMode()) {

				case zp: // lda zp -> lda offset,s
					{
						Instruction instr(m65816, op.mnemonic());
						if (!instr.hasAddressMode(stack_relative))
							return false;

						if (!e->is_register() && !e->is_integer())
							return false;
						break;
					}
				case zp_indirect_y: // lda (zp),y -> lda (offset,s),y 
					{
						Instruction instr(m65816, op.mnemonic());
						if (!instr.hasAddressMode(stack_relative_y))
							return false;

						if (!e->is_register() && !e->is_integer())
							return false;
						break;
					}
				case zp_x:
				case zp_y:
				case zp_indirect:
				case zp_indirect_x:
				case zp_indirect_z:
				case zp_relative:

				case zp_indirect_long:
				case zp_indirect_long_y:
						return false;

				case immediate:
					if (e->is_register())
						return false;
					break;

				default:
					break;;
			}
		}
	}
	return true;
}

void make_sr(BasicLine *line, AddressMode mode) {

	OpCode op = line->opcode;
	line->opcode = OpCode(m65816, op.mnemonic(), mode);

	// dp value is same as stack relative value.
	// crap -- also need to track the current stack adjustment --- 
	// pha
	// lda 1,s --> lda 3,s
	//
}

void convert_to_sr(BlockQueue &blocks) {


	for (auto block : blocks) {

		for (auto line : block->lines) {

			const OpCode &op = line->opcode;

			switch (op.addressMode()) {

				case zp: // lda zp -> lda offset,s
					make_sr(line, stack_relative);
					break;

				case zp_indirect_y: // lda (zp),y -> lda (offset,s),y 
					make_sr(line, stack_relative_y);
					break;

				case zp_x:
				case zp_y:
				case zp_indirect:
				case zp_indirect_x:
				case zp_indirect_z:
				case zp_relative:

				case zp_indirect_long:
				case zp_indirect_long_y:
						throw std::runtime_error("cannot convert to stack_relative address mode.");

				default:
					break;;

			}
		}
	}
}

bool reads_dp(BlockQueue &blocks) {

	for (auto block : blocks) {

		for (auto line : block->lines) {

			const OpCode &op = line->opcode;

			if (op.reads_zp()) return true;
			if (op.writes_zp()) return true;

		}
	}

	return false;
}

void assign_registers(Segment *segment, BlockQueue &blocks) {

	Expression::register_info ri;

	unsigned rtlb = 0;
	if (segment->rts) rtlb = 2;
	else rtlb = 3;

	if (segment->databank) rtlb++;

	// if no registers used, no need to set/restore dp.

	bool make_sr = can_convert_to_sr(blocks);
	bool save_d = reads_dp(blocks);
	if (make_sr) save_d = false;

	// pragma locals=...
	// pragma parameters=...
	// only need to worry about %t and %r.

	std::unordered_set<dp_register> rset;

	for (auto block : blocks) {
		dp_register r;

		for (auto line : block->lines) {

			// line already has list of registers.
			if (line->reg_status) {
				dp_register r = line->reg;
				if (r.type == 'r' || r.type == 't') {

					for (unsigned i = 0; i < line->reg_count; ++i) {
						rset.insert(r);
						r += 2;
					}

				}
			}
		}
	}

	// now convert to a vector and sort.
	std::vector<dp_register> rvector;
	std::vector<dp_register> tvector;

	segment->temp_size = 2 * rset.size();

	rvector.reserve(rset.size());
	tvector.reserve(rset.size());

	for (auto r : rset) {
		if (r.type == 'r')
			rvector.push_back(r);
		if (r.type == 't')
			tvector.push_back(r);
	}

	std::sort(rvector.begin(), rvector.end());
	std::sort(tvector.begin(), tvector.end());


	unsigned offset = 1;
	for (auto r : tvector) {
		ri.trmap.emplace(r, offset);
		offset += 2;
	}

	for (auto r : rvector) {
		ri.trmap.emplace(r, offset);
		offset += 2;
	}

	// locals
	ri.vbase = offset;
	offset += segment->local_size;

	// offset = total local parms.

	offset += rtlb;
	ri.pbase = offset;



	for (auto block : blocks) {

		for (auto line : block->lines) {

			if (line->reg_status) {
				line->operands[0] = line->operands[0]->assign_registers(ri);
			}
		}
	}

	// convert to sr...
	if (make_sr) convert_to_sr(blocks);


	// can generate epilogue / prologue now.

	if (segment->convention == Segment::naked) return;

	std::vector<BasicLine *> tmp;
	if (segment->debug && segment->name && !segment->name->empty()) {
		// tn 103 debug name.
		/*
		 * 82 xx xx                     brl  pastName
		 * 71 77                        dc.w $7771
		 * nn xx xx xx xx...            str  'the name string'
       */
		std::vector<ExpressionPtr> bytes;
		auto name = segment->name;
		bytes.push_back(Expression::Integer(0x71));
		bytes.push_back(Expression::Integer(0x77));
		bytes.push_back(Expression::Integer(name->length()));

		std::transform(name->begin(), name->end(), std::back_inserter(bytes), [](char c){
			return Expression::Integer(c);
		});


		tmp.push_back(new BasicLine(BRL, relative_long, star_plus(3 + bytes.size())));
		tmp.push_back(new BasicLine(DCB, Expression::Vector(std::move(bytes))));
	}


	if (segment->databank) {
		tmp.push_back(new BasicLine(PHB, implied));
		tmp.push_back(new BasicLine(PEA, absolute, 
			Expression::Binary('>>', 
				Expression::Identifier("~globals"), Expression::Integer(8))));
		tmp.push_back(new BasicLine(PLB, implied));
		tmp.push_back(new BasicLine(PLB, implied));

	}

	unsigned locals = segment->temp_size + segment->local_size;
	if (locals || segment->parm_size) {
		if (locals <= 8) {
			for (unsigned i = 0; i < locals; i += 2)
				tmp.push_back(new BasicLine(PHY, implied));


			if (save_d) {
				tmp.push_back(new BasicLine(TSC, implied));
				tmp.push_back(new BasicLine(PHD, implied));
				tmp.push_back(new BasicLine(TCD, implied));
			}
		}
		else {
			tmp.push_back(new BasicLine(TSC, implied));
			tmp.push_back(new BasicLine(SEC, implied));
			tmp.push_back(new BasicLine(SBC, immediate, Expression::Integer(locals)));
			tmp.push_back(new BasicLine(TCS, implied));

			if (save_d) {
				tmp.push_back(new BasicLine(PHD, implied));
				tmp.push_back(new BasicLine(TCD, implied));
			}
		}
	}
	segment->prologue_code = std::move(tmp);
	tmp.clear();

	if ((segment->convention == Segment::stdcall || segment->convention == Segment::pascal) && segment->parm_size) {

		unsigned xfer = rtlb;
		unsigned dest = locals + rtlb + segment->parm_size;

		AddressMode mode = zp;
		if (make_sr) mode = stack_relative;

		// 3 bytes...
		if (xfer & 0x01) {

			tmp.push_back(new BasicLine(LDA, mode, Expression::Integer(1 + locals + xfer - 2)));
			tmp.push_back(new BasicLine(STA, mode, Expression::Integer(1 + dest - 2)));

			xfer -= 1;
			dest -= 1;
		}

		// move the return address.
		while (xfer) {

			tmp.push_back(new BasicLine(LDA, mode, Expression::Integer(1 + locals + xfer - 2)));
			tmp.push_back(new BasicLine(STA, mode, Expression::Integer(1 + dest - 2)));
			xfer -= 2;
			dest -= 2;
		}

		locals += segment->parm_size;
	}

	// prologue...
	if (locals || segment->parm_size) {
		if (save_d) {
			tmp.push_back(new BasicLine(PLD, implied));
		}

		if (locals <= 8) {
			for (unsigned i = 0; i < locals; i += 2)
				tmp.push_back(new BasicLine(PLY, implied));
		} else {
			tmp.push_back(new BasicLine(TAY, implied));
			tmp.push_back(new BasicLine(TSC, implied));
			tmp.push_back(new BasicLine(CLC, implied));
			tmp.push_back(new BasicLine(ADC, immediate, Expression::Integer(locals)));
			tmp.push_back(new BasicLine(TCS, implied));
			tmp.push_back(new BasicLine(TYA, implied));
		}
	}

	if (segment->databank)
		tmp.push_back(new BasicLine(PLB, implied));

	if (segment->rts)
		tmp.push_back(new BasicLine(RTS, implied));
	else
		tmp.push_back(new BasicLine(RTL, implied));

	segment->epilogue_code = std::move(tmp);

}



