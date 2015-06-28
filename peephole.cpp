
#include <array>
#include <deque>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"
#include "OpCode.h"
#include "register_set.h"

template<class FX>
bool match(LineQueue &list, Mnemonic m1, Mnemonic m2, FX fx) {
	const unsigned Size = 2;
	Mnemonic mm[Size] = { m1, m2};
	BasicLine *lines[Size] = {0};

	if (list.size() < Size) return false;

	auto iter = list.begin();
	for (unsigned i = 0; i < Size; ++i, ++iter) {
		BasicLine *tmp = *iter;
		if (tmp->opcode.mnemonic() != mm[i]) return false;
		lines[i] = tmp;
	}

	return fx(lines[0], lines[1]);
}

template<class FX>
bool match(LineQueue &list, Mnemonic m1, Mnemonic m2, Mnemonic m3, FX fx) {
	const unsigned Size = 3;
	Mnemonic mm[Size] = { m1, m2, m3};
	BasicLine *lines[Size] = {0};

	if (list.size() < Size) return false;

	auto iter = list.begin();
	for (unsigned i = 0; i < Size; ++i, ++iter) {
		BasicLine *tmp = *iter;
		if (tmp->opcode.mnemonic() != mm[i]) return false;
		lines[i] = tmp;
	}

	return fx(lines[0], lines[1], lines[2]);
}


template<class FX>
bool match(LineQueue &list, Mnemonic m1, Mnemonic m2, Mnemonic m3, Mnemonic m4, FX fx) {
	const unsigned Size = 4;
	Mnemonic mm[Size] = { m1, m2, m3, m4};
	BasicLine *lines[Size] = {0};

	if (list.size() < Size) return false;

	auto iter = list.begin();
	for (unsigned i = 0; i < Size; ++i, ++iter) {
		BasicLine *tmp = *iter;
		if (tmp->opcode.mnemonic() != mm[i]) return false;
		lines[i] = tmp;
	}

	return fx(lines[0], lines[1], lines[2], lines[3]);
}



bool peephole(LineQueue &list) {


	/*
	 * n.b. - one pass can't catch everthing. consider:
	 * lda 
	 * 1 sta %t0
	 * 2 lda xxx
	 * 3 lda %t0
	 * (which can happen after lifetime analysis).
	 * pass 1 will kill line 2.  Pass 2 will kill line 3.
	 */

	LineQueue optimized;

	uint32_t starting_size = list.size();

	while(!list.empty()) {

		BasicLine *line = list.front();

		switch(line->opcode.mnemonic()) {
		default: break;

		case ADC:

			#if 0
			if (match(list, ADC, INC, INC, [&](BasicLine *a, BasicLine *b, BasicLine *c){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != implied) return false;
				if (c->opcode.addressMode() != implied) return false;

				uint32_t value;
				if (a->operands[0]->is_integer(value)) {
					a->operands[0] = Expression::Integer(value + 2);
					list.pop_front();
					list.pop_front();
					list.pop_front();
					list.push_front(a);
					delete b;
					delete c;

					return true;
				}
				return false;

			})) continue;
			#endif
			/* ADC const, inc a -> adc const+1 */
			// n.b. if carry flag is relevant, this could affect it */
			if (match(list, ADC, INC,[&](BasicLine *a, BasicLine *b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != implied) return false;

				uint32_t value;
				if (a->operands[0]->is_integer(value)) {
					a->operands[0] = Expression::Integer(value + 1);
					list.pop_front();
					list.pop_front();
					list.push_front(a);
					delete b;

					return true;
				}
				return false;

			})) continue;

			break;


		case AND:
			/* AND #const1, AND #const2 -> AND #(const1 | const 2) */
			if (match(list, AND, AND, [&](BasicLine *a, BasicLine *b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					uint32_t nv = (value_a & value_b) & 0xffff;

					if (nv == 0xffff) {
						list.pop_front();
						list.pop_front();

						delete a;
						delete b;
					} else {
						list.pop_front();
						delete a;
						b->operands[0] = Expression::Integer(nv);
					}
					return true;
				}
				return false;
			})) continue;

			break;


		case EOR:
			/* EOR #const1, EOR #const2 -> EOR #(const1 ^ const 2) */
			if (match(list, EOR, EOR, [&](BasicLine *a, BasicLine *b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					uint32_t nv = (value_a ^ value_b) & 0xffff;

					if (nv == 0) {
						list.pop_front();
						list.pop_front();

						delete a;
						delete b;
					} else {
						list.pop_front();
						delete a;
						b->operands[0] = Expression::Integer(nv);
					}
					return true;
				}
				return false;
			})) continue;

			break;



		case LDA:
			/* LDA #const, XBA -> LDA #tsnoc */
			if (match(list, LDA, XBA, [&](BasicLine *a, BasicLine *b){
				uint32_t value;
				if (a->opcode.addressMode() == immediate && a->operands[0]->is_integer(value)) {
					value &= 0xffff;
					value = (value << 8) | (value >> 8);
					value &= 0xffff;
					a->operands[0] = Expression::Integer(value);

					list.pop_front();
					list.pop_front();
					delete b;
					list.push_front(a);
					return true;
				}
				return false;
			})) continue;

			/* LDA xxx, PLA -> PLA */
			if (match(list, LDA, PLA, [&](BasicLine *a, BasicLine *b){
				// if a is immediate or zp ?
				list.pop_front();
				delete a;
				return true;
			})) continue;


			/* LDA xxx, LDA yyy -> LDA yyy */
			if (match(list, LDA, LDA, [&](BasicLine *a, BasicLine *b){
				// what if volatile memory?
				list.pop_front();
				delete a;
				return true;
			})) continue;

			/* lda xxx, CMP #0 -> lda xxx */
			// not safe... doesn't set carry flag 
			#if 0
			if (match(list, LDA, CMP, [&](BasicLine *a, BasicLine *b){
				uint32_t value;
				if (b->opcode.addressMode() == immediate && b->operands[0]->is_integer(value)) {
					if (value == 0) {
						list.pop_front();
						list.pop_front();
						delete b;
						list.push_front(a);
						return true;
					}
				}
				return false;
			})) continue;
			#endif


			/* LDA #const, AND #const -> LDA #const */
			if (match(list, LDA, AND, [&](BasicLine *a, BasicLine *b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					list.pop_front();

					a->operands[0] = Expression::Integer(value_a & value_b);
					list.push_front(a);

					delete b;
					return true;
				}
				return false;
			})) continue;

			/* LDA #const, ORA #const -> LDA #const */
			if (match(list, LDA, ORA, [&](BasicLine *a, BasicLine *b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					list.pop_front();

					a->operands[0] = Expression::Integer(value_a | value_b);
					list.push_front(a);

					delete b;
					return true;
				}
				return false;
			})) continue;

			/* LDA #const, EOR #const -> LDA #const */
			if (match(list, LDA, EOR, [&](BasicLine *a, BasicLine *b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					list.pop_front();

					a->operands[0] = Expression::Integer(value_a ^ value_b);
					list.push_front(a);

					delete b;
					return true;
				}
				return false;
			})) continue;


			break;


		case ORA:
			/* ORA #const1, ORA #const2 -> ORA #(const1 | const 2) */
			if (match(list, ORA, ORA, [&](BasicLine *a, BasicLine *b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					uint32_t nv = (value_a | value_b) & 0xffff;

					if (nv == 0) {
						list.pop_front();
						list.pop_front();

						delete a;
						delete b;
					} else {
						list.pop_front();
						delete a;
						b->operands[0] = Expression::Integer(nv);
					}
					return true;
				}
				return false;
			})) continue;

			break;


		case PEA:
			/* PEA xxx, PLA -> LDA #xxx */
			if (match(list, PEA, PLA, [&](BasicLine *a, BasicLine *b){

				BasicLine *tmp = new BasicLine(LDA, immediate, a->operands[0]);
				tmp->calc_registers();

				list.pop_front();
				list.pop_front();

				delete a;
				delete b;

				list.push_front(tmp);
				return true;
			})) continue;
			break;

		case PEI:
			/* PEI (xxx), PLA -> LDA <xxx */
			if (match(list, PEI, PLA, [&](BasicLine *a, BasicLine *b){

				BasicLine *tmp = new BasicLine(LDA, zp, a->operands[0]);
				tmp->calc_registers();

				list.pop_front();
				list.pop_front();

				delete a;
				delete b;

				list.push_front(tmp);
				return true;
			})) continue;
			break;

		case PHA:
			/* PHA, PLA -> NOP */
			if (match(list, PHA, PLA, [&](BasicLine *a, BasicLine *b){
				list.pop_front(); // PHA
				list.pop_front(); // PLA
				delete a;
				delete b;
				return true;
			})) continue;

			/* PHA, PLX -> TAX */
			// nb. won't work if m/x doesn't match.
			if (match(list, PHA, PLX, [&](BasicLine *a, BasicLine *b){


				BasicLine *tmp = new BasicLine(TAX, implied);

				list.pop_front();
				list.pop_front();
				delete a;
				delete b;
				list.push_front(tmp);
				return true;
			})) continue;

			/* PHA, PLY -> TAY */
			// nb. won't work if m/x doesn't match.
			if (match(list, PHA, PLY, [&](BasicLine *a, BasicLine *b){


				BasicLine *tmp = new BasicLine(TAY, implied);

				list.pop_front();
				list.pop_front();
				delete a;
				delete b;
				list.push_front(tmp);
				return true;
			})) continue;


			break;



		case PHX:
			/* PHX, PLX -> NOP */
			if (match(list, PHX, PLX, [&](BasicLine *a, BasicLine *b){
				list.pop_front(); // PHX
				list.pop_front(); // PLX
				delete a;
				delete b;
				return true;
			})) continue;
			break;

		case PHY:
			/* PHY, PLY -> NOP */
			if (match(list, PHY, PLY, [&](BasicLine *a, BasicLine *b){
				list.pop_front(); // PHY
				list.pop_front(); // PLY
				delete a;
				delete b;
				return true;
			})) continue;
			break;

#if 0
		// first can be dropped... need to check m/x bits
		// before dropping both.
		case REP:
			/* 	rep #$20, sep #$20 -> nop */
			if (match(list, REP, SEP, [&](BasicLine *a, BasicLine *b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					if (value_a == value_b) {
						list.pop_front(); // REP
						list.pop_front(); // SEP
						delete a;
						delete b;
						return true;
					}

				}
				return false;
			})) continue;
			break;

		case SEP:
			/* 	sep #$20, rep #$20 -> nop */
			if (match(list, REP, SEP, [&](BasicLine *a, BasicLine *b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					if (value_a == value_b) {
						list.pop_front(); // SEP
						list.pop_front(); // REP
						delete a;
						delete b;
						return true;
					}

				}
				return false;
			})) continue;
			break;
#endif

		case STA:
			/* STA %t0, LDA %t0 -> STA %t0 */
			if (match(list, STA, LDA, [&](BasicLine *a, BasicLine *b){
				//printf("STA %d/ LDA %d\n" , a->opcode.addressMode(), b->opcode.addressMode());

				if (a->opcode.addressMode() == b->opcode.addressMode()) {

					dp_register reg_a, reg_b;
					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {
							list.pop_front(); // a
							list.pop_front(); // b
							delete b;
							list.push_front(a);
							return true;
						}
					}
				}
				return false;
			})) continue;

			/* STA %t0, LDX %t0 -> STA %t0, TAX */
			if (match(list, STA, LDX, [&](BasicLine *a, BasicLine *b){
				//printf("STA %d/ LDA %d\n" , a->opcode.addressMode(), b->opcode.addressMode());

				if (a->opcode.addressMode() == b->opcode.addressMode()) {

					dp_register reg_a, reg_b;
					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {
							list.pop_front(); // a
							list.pop_front(); // b
							delete b;
							BasicLine *tmp = new BasicLine(TAX, implied);

							list.push_front(tmp);
							list.push_front(a);
							return true;
						}
					}
				}
				return false;
			})) continue;

			/* STA %t0, LDY %t0 -> STA %t0, TAY */
			if (match(list, STA, LDX, [&](BasicLine *a, BasicLine *b){
				//printf("STA %d/ LDA %d\n" , a->opcode.addressMode(), b->opcode.addressMode());

				if (a->opcode.addressMode() == b->opcode.addressMode()) {

					dp_register reg_a, reg_b;
					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {
							list.pop_front(); // a
							list.pop_front(); // b
							delete b;
							BasicLine *tmp = new BasicLine(TAY, implied);

							list.push_front(tmp);
							list.push_front(a);
							return true;
						}
					}
				}
				return false;
			})) continue;


			/* STA %t0, PEI (<%t0) -> STA %t0, PHA */
			if (match(list, STA, PEI, [&](BasicLine *a, BasicLine *b){
				dp_register reg_a, reg_b;

				if (a->opcode.addressMode() != zp) return false;

				if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
					if (reg_a == reg_b) {

						BasicLine *tmp = new BasicLine(PHA, implied);

						list.pop_front(); // a
						list.pop_front(); // b
						delete b;
						/*
						list.push_front(tmp);
						list.push_front(a);
						*/
						list.insert(list.begin(), {a, tmp});
						return true;
					}
				}
				return false;
			})) continue;

			/* STA %t0, PLY, LDA %t0  -> STA %t0, PLY */
			/* PLY is used to clean the stack after a cdecl call */
			if (match(list, STA, PLY, LDA, [&](BasicLine *a, BasicLine *b, BasicLine *c){
				dp_register reg_a, reg_c;

				if (a->opcode.addressMode() != zp) return false;
				if (c->opcode.addressMode() != zp) return false;

				if (a->operands[0]->is_register(reg_a) && c->operands[0]->is_register(reg_c)) {
					if (reg_a == reg_c) {

						list.pop_front(); // a
						list.pop_front(); // b
						list.pop_front(); //c
						list.insert(list.begin(), {a, b});
						delete c;
						return true;
					}
				}
				return false;
			})) continue;


			/* STA %t0, STA %t2, PEI %t0 */
			// should handle this elsewhere?
			if (match(list, STA, STA, PEI, [&](BasicLine *a, BasicLine *b, BasicLine *c){
				dp_register reg_a, reg_c;

				if (a->opcode.addressMode() != zp) return false;

				if (a->operands[0]->is_register(reg_a) && c->operands[0]->is_register(reg_c)) {
					if (reg_a == reg_c) {
						BasicLine *tmp = new BasicLine(PHA, implied);

						list.pop_front(); // a
						list.pop_front(); // b
						list.pop_front(); //c

						delete c;

						list.insert(list.begin(), {a, b, tmp});
						return true;
					}
				}
				return false;
			})) continue;


			/* STA %t0, LDA xxx, CLC, ADC %t0 -> STA %t0, CLC, ADC xxx */
			// works for dp or immediate (or absolute...)
			if (match(list, STA, LDA, CLC, ADC, 
				[&](BasicLine *a, BasicLine *b, BasicLine *c, BasicLine *d){
					dp_register reg_a, reg_d;
					if (a->opcode.addressMode() != zp) return false;
					if (d->opcode.addressMode() != zp) return false;

					if (a->operands[0]->is_register(reg_a)
						&& d->operands[0]->is_register(reg_d)
					) {
						if (reg_a != reg_d) return false;

						list.pop_front();
						list.pop_front();
						list.pop_front();
						list.pop_front();

						BasicLine *tmp = new BasicLine(ADC, b->opcode.addressMode(), b->operands[0]);
						tmp->calc_registers();
						list.insert(list.begin(), {a, c, tmp});

						delete b;
						delete d;
						return true;
					}
					return false;

			})) continue;

			// sta %t0, rep #20, lda %t0
			// ... 


			break;


		case TAX:
			/* TAX, PLX -> PLX */
			if (match(list, TAX, PLX, [&](BasicLine *a, BasicLine *b){

				list.pop_front(); // a
				delete a;

				return true;
			})) continue;	

		case BRA:
			/* bra label, label: -> label */
			if (match(list, BRA, kUndefinedMnemonic, [&](BasicLine *a, BasicLine *b){
				const std::string *label;
				if (b->label && a->operands[0]->is_identifier(label)) {
					if (label == b->label) {
						list.pop_front();
						delete a;
						return true;
					}
				}
				return false;
			})) continue;
			break;

		case BRL:
			/* brl label, label: -> label */
			if (match(list, BRL, kUndefinedMnemonic, [&](BasicLine *a, BasicLine *b){
				const std::string *label;
				if (b->label && a->operands[0]->is_identifier(label)) {
					if (label == b->label) {
						list.pop_front();
						delete a;
						return true;
					}
				}
				return false;
			})) continue;
			break;

		case JMP:
			/* brl label, label: -> label */
			if (match(list, JMP, kUndefinedMnemonic, [&](BasicLine *a, BasicLine *b){
				const std::string *label;
				if (b->opcode.addressMode() == absolute 
					&& b->label 
					&& a->operands[0]->is_identifier(label)) {
					if (label == b->label) {
						list.pop_front();
						delete a;
						return true;
					}
				}
				return false;
			})) continue;
			break;


		}

		list.pop_front();
		optimized.push_back(line);
	}


	list = std::move(optimized);
	return (list.size() != starting_size);
}



bool final_peephole(LineQueue &list) {
	/*
	 * last-chance optimizations
	 *
	 */

	// hmmm can't just drop CMP #0 since CMP sets the c flag,
	// LDA does not.  CMP #0 bcs ... == if (x >= 0)
	// but bcs only useful for unsigned... shouldn't ever 
	// check against const #0 unsigned never < 0, unsigned always >= 0

	LineQueue optimized;

	uint32_t starting_size = list.size();

	while(!list.empty()) {

		BasicLine *line = list.front();

		switch(line->opcode.mnemonic()) {
		default: break;

		case LDA:
			/* LDA xxx, CMP #0 -> LDA xxx */
			if (match(list, LDA, CMP, [&](BasicLine *a, BasicLine *b){
				uint32_t value;
				if (b->opcode.addressMode() == immediate && b->operands[0]->is_integer(value)) {
					if (value == 0) {
						list.pop_front();
						list.pop_front();
						list.push_front(a);
						delete b;
						return true;
					}
				}
				return false;
			})) continue;
			break;

		case LDX:
			/* LDX xxx, CPX #0 -> LDX xxx */
			if (match(list, LDX, CPX, [&](BasicLine *a, BasicLine *b){
				uint32_t value;
				if (b->opcode.addressMode() == immediate && b->operands[0]->is_integer(value)) {
					if (value == 0) {
						list.pop_front();
						list.pop_front();
						list.push_front(a);
						delete b;
						return true;
					}
				}
				return false;
			})) continue;
			break;

		case LDY:
			/* LDY xxx, CPY #0 -> LDY xxx */
			if (match(list, LDY, CPY, [&](BasicLine *a, BasicLine *b){
				uint32_t value;
				if (b->opcode.addressMode() == immediate && b->operands[0]->is_integer(value)) {
					if (value == 0) {
						list.pop_front();
						list.pop_front();
						list.push_front(a);
						delete b;
						return true;
					}
				}
				return false;
			})) continue;
			break;

		}


		list.pop_front();
		optimized.push_back(line);
	}


	list = std::move(optimized);
	return (list.size() != starting_size);

}
