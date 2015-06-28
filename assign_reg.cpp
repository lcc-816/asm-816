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

void assign_registers(Segment *segment, BlockQueue &blocks) {

	Expression::register_info ri;

	unsigned rtlb = 0;
	if (segment->rts) rtlb = 2;
	else rtlb = 3;

	if (segment->databank) rtlb++;

	// if no registers used, no need to set/restore dp.


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
			tmp.push_back(new BasicLine(TSC, implied));
			tmp.push_back(new BasicLine(PHD, implied));
			tmp.push_back(new BasicLine(TCD, implied));
		}
		else {
			tmp.push_back(new BasicLine(TSC, implied));
			tmp.push_back(new BasicLine(SEC, implied));
			tmp.push_back(new BasicLine(SBC, immediate, Expression::Integer(locals)));
			tmp.push_back(new BasicLine(TCS, implied));
			tmp.push_back(new BasicLine(PHD, implied));
			tmp.push_back(new BasicLine(TCD, implied));
		}
	}
	segment->prologue_code = std::move(tmp);
	tmp.clear();

	if (segment->convention == Segment::stdcall || segment->convention == Segment::pascal) {

		unsigned xfer = (rtlb + 1 ) & ~0x01;
		unsigned dest = locals + rtlb + segment->parm_size;

		// move the return address.
		while (xfer) {
			tmp.push_back(new BasicLine(LDA, zp, Expression::Integer(1 + locals + xfer - 2)));
			tmp.push_back(new BasicLine(STA, zp, Expression::Integer(1 + dest - 2)));
			xfer -= 2;
			dest -= 2;
		}

		locals += segment->parm_size;
	}

	// prologue...
	if (locals || segment->parm_size) {
		tmp.push_back(new BasicLine(PLD, implied));

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