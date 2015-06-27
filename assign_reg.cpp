#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Expression.h"
#include "register_set.h"

#include "common.h"
#include "Machine.h"

#define __phd() tmp.push_back(0x0b);
#define __pld() tmp.push_back(0x2b);

#define __ply() tmp.push_back(0x7a);
#define __phy() tmp.push_back(0x5a);


#define __tay() tmp.push_back(0xab);
#define __tya() tmp.push_back(0x98);


#define __tsc() tmp.push_back(0x3b);
#define __tcs() tmp.push_back(0x1b);
#define __tcd() tmp.push_back(0x5b);

#define __clc() tmp.push_back(0x18);
#define __sec() tmp.push_back(0x38);



#define __rtl() tmp.push_back(0x6b);
#define __rts() tmp.push_back(0x60);

#define __adc_imm(arg) { uint16_t x = arg; \
	tmp.push_back(0x69); \
	tmp.push_back((x >> 0) & 0xff); \
	tmp.push_back((x >> 8) & 0xff); \
}

#define __sbc_imm(arg) { uint16_t x = arg; \
	tmp.push_back(0xe9); \
	tmp.push_back((x >> 0) & 0xff); \
	tmp.push_back((x >> 8) & 0xff); \
}


#define __lda_dp(arg) tmp.push_back(0xa5); tmp.push_back((arg) & 0xff);
#define __sta_dp(arg) tmp.push_back(0x85); tmp.push_back((arg) & 0xff);

#define __phb() tmp.push_back(0x8b);
#define __plb() tmp.push_back(0xab);
#define __pea() tmp.push_back(0xf4);


/*
 * assign a dp register location for all %t, %r, %v, %p dp_registers. 
 * This also generates epilogue / prologue code.
 *
 */

void assign_registers(Segment *segment) {

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

	for (auto block : segment->blocks) {
		dp_register r;

		for (auto line : block->lines) {

			// line already has list of registers.
			if (line->reg) {
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



	for (auto block : segment->blocks) {

		for (auto line : block->lines) {

			if (line->reg) {
				line->operands[0] = line->operands[0]->assign_registers(ri);
			}
		}
	}

	// can generate epilogue / prologue now.

	if (segment->convention == Segment::naked) return;


	std::vector<BasicLine *> tmp;
	if (segment->databank) {
		tmp.push_back(new BasicLine(PHB, implied));
		tmp.push_back(new BasicLine(PEA, absolute, 
			Expression::Binary('<<', 
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

	segment->epilogue_code = std::move(tmp);




#if 0
	//std::vector<uint8_t> tmp;

	if (segment->databank) __phb()
	// also generate pea ~globals >> 8; plb; plb ? this should probably just be done in lcc.

	unsigned locals = segment->temp_size + segment->local_size;
	if (locals || segment->parm_size) {
		if (locals <= 8) {
			for (unsigned i = 0; i < locals; i += 2)
				__phy()
			__tsc()
			__phd()
			__tcd()
		}
		else {
			__tsc()
			__sec()
			__sbc_imm(locals)
			__tcs()
			__phd()
			__tcd()
		}
	}

	segment->prologue_code = std::move(tmp);
	tmp.clear();

	if (segment->convention == Segment::stdcall || segment->convention == Segment::pascal) {

		unsigned xfer = (rtlb + 1 ) & ~0x01;
		unsigned dest = locals + rtlb + segment->parm_size;

		// move the return address.
		while (xfer) {

			__lda_dp(1 + locals + xfer - 2)
			__sta_dp(1 + dest - 2)
			xfer -= 2;
			dest -= 2;
		}

		locals += segment->parm_size;
	}

	// prologue...
	if (locals || segment->parm_size) {
		__pld()

		if (locals <= 8) {
			for (unsigned i = 0; i < locals; i += 2) {
				__ply()
		}
		} else {
			__tay() // save
			__tsc()
			__clc()
			__adc_imm(locals)
			__tcs()
			__tya()
		}


	}

	if (segment->databank)
		__plb()

	segment->epilogue_code = std::move(tmp);
	tmp.clear();
#endif

}