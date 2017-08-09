
#include <array>
#include <deque>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"
#include "OpCode.h"
#include "dp_register_set.h"

/*
inline OpCode operator/(Mnemonic m, AddressMode mode) {
	OpCode op(m65816, m, mode);
	if (op.mnemonic() == kUndefinedMnemonic) throw std::runtime_error("Invalid opcode");
	return op;
}
*/

namespace {
	struct match_any_t {};
	static const match_any_t _{};

	struct match_eof_t {};
	static const match_eof_t eof{};
}

typedef std::pair<Mnemonic, AddressMode> ma_pair;
typedef std::pair<Mnemonic, Mnemonic> mm_pair;

inline ma_pair operator/(Mnemonic m, AddressMode mode) {
	return std::make_pair(m, mode);
}

inline std::pair<Mnemonic, Mnemonic> operator|(Mnemonic m1, Mnemonic m2) {
	return std::make_pair(m1, m2);
}

template<class T1, class T2>
inline std::pair<std::pair<T1, T2>, Mnemonic> operator|(std::pair<T1, T2> lhs, Mnemonic rhs) {
	return std::make_pair(lhs, rhs);
}

template<class T1, class T2>
inline std::pair<std::pair<T1, T2>, ma_pair> operator|(std::pair<T1, T2> lhs, ma_pair rhs) {
	return std::make_pair(lhs, rhs);
}


bool matches(const BasicLine &line, Mnemonic m) {
	return line.opcode.mnemonic() == m;
}

bool matches(const BasicLine &line, Directive d) {
	return line.directive == d;
}

bool matches(const BasicLine &line, match_any_t) {
	return true;
}


bool matches(const BasicLine &line, const OpCode &op) {
	return line.opcode == op;
}

bool matches(const BasicLine &line, const std::pair<Mnemonic, AddressMode> &mm) {
	return line.opcode.mnemonic() == mm.first && line.opcode.addressMode() == mm.second;
}

bool matches(const BasicLine &line, const std::pair<Mnemonic, Mnemonic> &mm) {
	Mnemonic m = line.opcode.mnemonic();
	return m == mm.first || m == mm.second;
}

// built up from a set of | ops.
template<class T1, class T2>
bool matches(const BasicLine &line, const std::pair<T1, T2> &p) {
	if (matches(line, p.first)) return true;
	if (matches(line, p.second)) return true;
	return false;
}


template<unsigned Offset=0, std::size_t N>
bool matches(const std::array<BasicLinePtr, N> &) {
	return true;
}

template<unsigned Offset=0, std::size_t N, class T, class ...Rest>
bool matches(const std::array<BasicLinePtr, N> &lines, T &&t, Rest&& ...args) {
	if (!matches(*lines[Offset], std::forward<T>(t))) return false;
	return matches<Offset+1>(lines, std::forward<Rest>(args)...);
}


template<unsigned N, class FX>
bool expand_fx(std::array<BasicLinePtr, N> &lines, FX);


template<class FX>
bool expand_fx(std::array<BasicLinePtr, 1> &lines, FX fx)
{
	return fx(lines[0]);
}


template<class FX>
bool expand_fx(std::array<BasicLinePtr, 2> &lines, FX fx)
{
	return fx(lines[0], lines[1]);
}

template<class FX>
bool expand_fx(std::array<BasicLinePtr, 3> &lines, FX fx)
{
	return fx(lines[0], lines[1], lines[2]);
}

template<class FX>
bool expand_fx(std::array<BasicLinePtr, 4> &lines, FX fx)
{
	return fx(lines[0], lines[1], lines[2], lines[3]);
}

template<class FX>
bool expand_fx(std::array<BasicLinePtr, 5> &lines, FX fx)
{
	return fx(lines[0], lines[1], lines[2], lines[3], lines[4]);
}


//#if 0
template<class ...Args, class FX>
bool match(LineQueue &list, Args && ...args, FX fx) {
	const unsigned Size = sizeof...(args);
	std::array<BasicLinePtr, Size> lines;

	if (list.size() < Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	//if (!matches(lines, m1, m2)) return false;
	if (!matches(lines, std::forward<Args>(args)...)) return false;
	return expand_fx(lines, fx);
}
//#endif

template<class A, class FX>
bool match(LineQueue &list, A m1, FX fx) {
	const unsigned Size = 1;
	std::array<BasicLinePtr, Size> lines;

	if (list.size() < Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1)) return false;

	return fx(lines[0]);
}


template<class A, class FX>
bool match(LineQueue &list, A m1, match_eof_t, FX fx) {
	const unsigned Size = 1;
	std::array<BasicLinePtr, Size> lines;

	if (list.size() != Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1)) return false;

	return fx(lines[0]);
}

template<class A, class B, class FX>
bool match(LineQueue &list, A m1, B m2, FX fx) {
	const unsigned Size = 2;
	std::array<BasicLinePtr, Size> lines;

	if (list.size() < Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1, m2)) return false;

	return fx(lines[0], lines[1]);
}

template<class A, class B, class FX>
bool match(LineQueue &list, A m1, B m2, match_eof_t, FX fx) {

	const unsigned Size = 2;

	std::array<BasicLinePtr, Size> lines;

	if (list.size() != Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1, m2)) return false;

	return fx(lines[0], lines[1]);
}

template<class A, class B, class C, class FX>
bool match(LineQueue &list, A m1, B m2, C m3, FX fx) {

	const unsigned Size = 3;

	std::array<BasicLinePtr, Size> lines;

	if (list.size() < Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1, m2, m3)) return false;

	return fx(lines[0], lines[1], lines[2]);
}



template<class A, class B, class C, class FX>
bool match(LineQueue &list, A m1, B m2, C m3, match_eof_t, FX fx) {
	const unsigned Size = 3;
	std::array<BasicLinePtr, Size> lines;

	if (list.size() != Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1, m2, m3)) return false;

	return fx(lines[0], lines[1], lines[2]);
}


template<class A, class B, class C, class D, class FX>
bool match(LineQueue &list, A m1, B m2, C m3, D m4, FX fx) {
	const unsigned Size = 4;
	std::array<BasicLinePtr, Size> lines;

	if (list.size() < Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1, m2, m3, m4)) return false;

	return fx(lines[0], lines[1], lines[2], lines[3]);
}


template<class A, class B, class C, class D, class FX>
bool match(LineQueue &list, A m1, B m2, C m3, D m4, match_eof_t, FX fx) {
	const unsigned Size = 4;
	std::array<BasicLinePtr, Size> lines;

	if (list.size() != Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1, m2, m3, m4)) return false;

	return fx(lines[0], lines[1], lines[2], lines[3]);
}


template<class A, class B, class C, class D, class E, class FX>
bool match(LineQueue &list, A m1, B m2, C m3, D m4, E m5, FX fx) {
	const unsigned Size = 5;
	std::array<BasicLinePtr, Size> lines;

	if (list.size() < Size) return false;

	std::copy_n(list.begin(), Size, lines.begin());

	if (!matches(lines, m1, m2, m3, m4, m5)) return false;

	return fx(lines[0], lines[1], lines[2], lines[3], lines[4]);
}



bool peephole(BasicBlockPtr block) {

	bool recalc = false;
	LineQueue list = std::move(block->lines);
	BasicLinePtr exit_branch = block->exit_branch;

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

		BasicLinePtr line = list.front();

		switch(line->opcode.mnemonic()) {
		default: break;

		case ADC:

			// ADC #%t0, inc, inc -> adc #%t2
			if (match(list, ADC/immediate, INC/implied, INC/implied, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){

				dp_register reg;
				if (a->operands[0]->is_register(reg)) {

					list.pop_front();
					list.pop_front();
					list.pop_front();

					reg += 2;
					a->operands[0] = Expression::Register(reg);
					a->calc_registers();
					list.push_front(a);

					return true;
				}
				return false;
			})) continue;

			#if 0
			if (match(list, ADC, INC, INC, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){
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

					return true;
				}
				return false;

			})) continue;
			/* ADC const, inc a -> adc const+1 */
			// n.b. if carry flag is relevant, this could affect it */
			if (match(list, ADC, INC,[&](BasicLinePtr a, BasicLinePtr b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != implied) return false;

				uint32_t value;
				if (a->operands[0]->is_integer(value)) {
					a->operands[0] = Expression::Integer(value + 1);
					list.pop_front();
					list.pop_front();
					list.push_front(a);

					return true;
				}
				return false;

			})) continue;
			#endif

			break;


		case AND:
			/* AND #const1, AND #const2 -> AND #(const1 & const 2) */
			if (match(list, AND, AND, [&](BasicLinePtr a, BasicLinePtr b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					uint32_t nv = (value_a & value_b) & 0xffff;

					if (nv == 0xffff) {
						list.pop_front();
						list.pop_front();

					} else {
						list.pop_front();
						b->operands[0] = Expression::Integer(nv);
					}
					return true;
				}
				return false;
			})) continue;

			/* AND #const, cmp #0, bmi  --> and #const */
			if (match(list, AND, CMP, eof, [&](BasicLinePtr a, BasicLinePtr b){
				uint32_t value_a, value_b;
				if (exit_branch && exit_branch->branch.type == branch::mi
					&& a->operands[0]->is_integer(value_a) 
					&& b->operands[0]->is_integer(value_b))
				{

					uint16_t x = 0xffff;
					x &= value_a;

					if (x & 0x8000) return false;
					list.pop_front();
					list.pop_front();
					list.push_front(a);
					block->exit_branch = nullptr;
					// also need to update the next set...
					recalc = true;
					return true;
				}
				return false;

			})) continue;

			/* AND #0 -> LDA #0 */
			if (match(list, AND/immediate, [&](BasicLinePtr a){
				uint32_t value;
				if (a->operands[0]->is_integer(value)) {
					if ((value & 0xffff) == 0) {
						BasicLinePtr tmp = BasicLine::Make(LDA, immediate, Expression::Integer(0));
						tmp->calc_registers();
						list.pop_front();
						list.push_front(tmp);
						return true;
					}
				}
				return false;
			})) continue;

			break;


		case EOR:
			/* EOR #const1, EOR #const2 -> EOR #(const1 ^ const 2) */
			if (match(list, EOR/immediate, EOR/immediate, [&](BasicLinePtr a, BasicLinePtr b){
				if (a->opcode.addressMode() != immediate) return false;
				if (b->opcode.addressMode() != immediate) return false;

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					uint32_t nv = (value_a ^ value_b) & 0xffff;

					if (nv == 0) {
						list.pop_front();
						list.pop_front();

					} else {
						list.pop_front();
						b->operands[0] = Expression::Integer(nv);
					}
					return true;
				}
				return false;
			})) continue;

#if 0
			/* EOR #-1, CLC, ADC #const -> SEC SBC #const-1 */
			/* oops, this is actually  (const-1)-a, so not optimizable here */
			if (match(list, EOR/immediate, CLC, ADC/immediate, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){
				uint32_t a_value, c_value;
				if (a->operands[0]->is_integer(a_value) && c->operands[0]->is_integer(c_value)) {
					if ((a_value & 0xffff) == 0xffff) {
						list.pop_front();
						list.pop_front();
						list.pop_front();
						auto sec = BasicLine::Make(SEC);
						auto sbc = BasicLine::MAKE(SBC, immediate, Expression::Integer(c_value - 1));
						list.insert(list.begin(), { sec, sbc });
					}
				}
				return false;
			})) continue;
#endif
			break;



		case LDA:
			/* LDA #const, XBA -> LDA #tsnoc */
			if (match(list, LDA, XBA, [&](BasicLinePtr a, BasicLinePtr b){
				uint32_t value;
				if (a->opcode.addressMode() == immediate && a->operands[0]->is_integer(value)) {
					value &= 0xffff;
					value = (value << 8) | (value >> 8);
					value &= 0xffff;
					a->operands[0] = Expression::Integer(value);

					list.pop_front();
					list.pop_front();
					list.push_front(a);
					return true;
				}
				return false;
			})) continue;

#if 0
	// now handled in register passes.
			/* LDA xxx, PLA -> PLA */
			if (match(list, LDA, PLA, [&](BasicLinePtr a, BasicLinePtr b){
				// if a is immediate or zp ?
				list.pop_front();
				return true;
			})) continue;


			/* LDA xxx, LDA yyy -> LDA yyy */
			if (match(list, LDA, LDA, [&](BasicLinePtr a, BasicLinePtr b){
				// what if volatile memory?
				list.pop_front();
				return true;
			})) continue;


#endif
			/* LDA #const, AND #const -> LDA #const */
			if (match(list, LDA/immediate, AND/immediate, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					list.pop_front();

					a->operands[0] = Expression::Integer(value_a & value_b);
					list.push_front(a);

					return true;
				}
				return false;
			})) continue;

			/* LDA #const, ORA #const -> LDA #const */
			if (match(list, LDA/immediate, ORA/immediate, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					list.pop_front();

					a->operands[0] = Expression::Integer(value_a | value_b);
					list.push_front(a);

					return true;
				}
				return false;
			})) continue;

			/* LDA #const, EOR #const -> LDA #const */
			if (match(list, LDA/immediate, EOR/immediate, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					list.pop_front();

					a->operands[0] = Expression::Integer(value_a ^ value_b);
					list.push_front(a);

					return true;
				}
				return false;
			})) continue;


			// LDX first to improve other optimizations.
			if (match(list, LDA, LDX, [&](BasicLinePtr a, BasicLinePtr b){
					dp_register reg_a, reg_e;

					AddressMode ma, mb;
					ma = a->opcode.addressMode();
					mb = b->opcode.addressMode();

					if (ma != zp && ma != immediate) return false;
					if (mb != zp && mb != immediate) return false;

					list.pop_front();
					list.pop_front();

					list.insert(list.begin(), {b, a});

					return true;

		 	})) continue;


			// LDA %t0, STA %t0 -- drop the store
			if (match(list, LDA/zp, STA/zp, [&](BasicLinePtr a, BasicLinePtr b){
				dp_register reg_a, reg_b;

				//if (a->opcode.addressMode() != zp) return false;
				//if (b->opcode.addressMode() != zp) return false;

				if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
					if (reg_a == reg_b) {
						list.pop_front();
						list.pop_front();
						list.push_front(a);

						return true;
					}
				}
				return false;
			})) continue;

			// LDA #0, sta reg, lda ...
			if (match(list, LDA/immediate, STA/zp, LDA, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){

				uint32_t int_a;
				dp_register reg_b;
				if (a->operands[0]->is_integer(int_a) && b->operands[0]->is_register(reg_b)) {
					if ((int_a & 0xffff) == 0) {
						list.pop_front();
						list.pop_front();

						BasicLinePtr tmp = BasicLine::Make(STZ, zp, b->operands[0]);
						tmp->calc_registers();

						list.push_front(tmp);

						return true;
					}
				}
				return false;

			})) continue;

			break;


		case ORA:
			/* ORA #const1, ORA #const2 -> ORA #(const1 | const 2) */
			if (match(list, ORA/immediate, ORA/immediate, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					uint32_t nv = (value_a | value_b) & 0xffff;

					if (nv == 0) {
						list.pop_front();
						list.pop_front();

					} else {
						list.pop_front();
						b->operands[0] = Expression::Integer(nv);
					}
					return true;
				}
				return false;
			})) continue;

			break;


		case PEA:
			/* PEA xxx, PLA -> LDA #xxx */
			if (match(list, PEA, PLA, [&](BasicLinePtr a, BasicLinePtr b){

				BasicLinePtr tmp = BasicLine::Make(LDA, immediate, a->operands[0]);
				tmp->calc_registers();

				list.pop_front();
				list.pop_front();

				list.push_front(tmp);
				return true;
			})) continue;
			break;

		case PEI:
			/* PEI (xxx), PLA -> LDA <xxx */
			if (match(list, PEI, PLA, [&](BasicLinePtr a, BasicLinePtr b){

				BasicLinePtr tmp = BasicLine::Make(LDA, zp, a->operands[0]);
				tmp->calc_registers();

				list.pop_front();
				list.pop_front();

				list.push_front(tmp);
				return true;
			})) continue;


			/* PEI (xxx), PLX -> LDX <xxx */
			if (match(list, PEI, PLX, [&](BasicLinePtr a, BasicLinePtr b){

				BasicLinePtr tmp = BasicLine::Make(LDX, zp, a->operands[0]);
				tmp->calc_registers();

				list.pop_front();
				list.pop_front();

				list.push_front(tmp);
				return true;
			})) continue;

			/* PEI (xxx), PLY -> LDY <xxx */
			if (match(list, PEI, PLX, [&](BasicLinePtr a, BasicLinePtr b){

				BasicLinePtr tmp = BasicLine::Make(LDY, zp, a->operands[0]);
				tmp->calc_registers();

				list.pop_front();
				list.pop_front();

				list.push_front(tmp);
				return true;
			})) continue;


			/* PEI (xxx), LDA xxx -> LDA <xxx, PHA */
			if (match(list, PEI, LDA, [&](BasicLinePtr a, BasicLinePtr b){

				dp_register reg_a, reg_b;
				if (b->opcode.addressMode() == zp 
					&& a->operands[0]->is_register(reg_a)
					&& b->operands[0]->is_register(reg_b)
				){
					if (reg_a == reg_b) {

						BasicLinePtr tmp = BasicLine::Make(PHA, implied);
						tmp->calc_registers();

						list.pop_front();
						list.pop_front();

						list.insert(list.begin(), {b, tmp});

						return true;
					}
				}
				return false;
			})) continue;

				
			break;


		case PHA:
			/* PHA, PLA -> NOP */
			if (match(list, PHA, PLA, [&](BasicLinePtr a, BasicLinePtr b){
				list.pop_front(); // PHA
				list.pop_front(); // PLA

				return true;
			})) continue;

			/* PHA, PLX -> TAX */
			// nb. won't work if m/x doesn't match.
			if (match(list, PHA, PLX, [&](BasicLinePtr a, BasicLinePtr b){


				BasicLinePtr tmp = BasicLine::Make(TAX, implied);

				list.pop_front();
				list.pop_front();

				list.push_front(tmp);
				return true;
			})) continue;

			/* PHA, PLY -> TAY */
			// nb. won't work if m/x doesn't match.
			if (match(list, PHA, PLY, [&](BasicLinePtr a, BasicLinePtr b){


				BasicLinePtr tmp = BasicLine::Make(TAY, implied);

				list.pop_front();
				list.pop_front();

				list.push_front(tmp);
				return true;
			})) continue;


			break;



		case PHX:
			/* PHX, PLX -> NOP */
			if (match(list, PHX, PLX, [&](BasicLinePtr a, BasicLinePtr b){
				list.pop_front(); // PHX
				list.pop_front(); // PLX

				return true;
			})) continue;
			break;

		case PHY:
			/* PHY, PLY -> NOP */
			if (match(list, PHY, PLY, [&](BasicLinePtr a, BasicLinePtr b){
				list.pop_front(); // PHY
				list.pop_front(); // PLY

				return true;
			})) continue;
			break;

		// TODO -- check m/x bits? before dropping?
		case REP:

			/* 	rep #a, rep #b -> rep #a|b */
			if (match(list, REP, REP, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					a->operands[0] = Expression::Integer(value_a | value_b);
					return true;
				}
				return false;
			})) continue;

			/* 	rep #$20, sep #$20 -> nop */
			if (match(list, REP, SEP, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					if (value_a == value_b) {
						list.pop_front(); // REP
						list.pop_front(); // SEP

						return true;
					}

				}
				return false;
			})) continue;
			break;
		case SEP:

			/* 	sep #a, sep #b -> sep #a|b */
			if (match(list, SEP, SEP, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					list.pop_front();
					a->operands[0] = Expression::Integer(value_a | value_b);
					return true;
				}
				return false;
			})) continue;


			/* 	sep #a, rep #b -> sep #a & ~b, rep #b */
			if (match(list, SEP, REP, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					if (value_a & value_b) {
						a->operands[0] = Expression::Integer(value_a & ~value_b);
						return true;
					}
				}
				return false;
			})) continue;

			/* 	sep #0 -> nop */
			if (match(list, SEP, [&](BasicLinePtr a){

				uint32_t value_a;
				if (a->operands[0]->is_integer(value_a) && value_a == 0) {

					list.pop_front();
					return true;
				}
				return false;
			})) continue;

			break;


#if 0
			/* 	sep #$20, rep #$20 -> nop */
			if (match(list, REP, SEP, [&](BasicLinePtr a, BasicLinePtr b){

				uint32_t value_a, value_b;
				if (a->operands[0]->is_integer(value_a) && b->operands[0]->is_integer(value_b)) {

					if (value_a == value_b) {
						list.pop_front(); // SEP
						list.pop_front(); // REP

						return true;
					}

				}
				return false;
			})) continue;
			break;
#endif

		case STA:
			/* STA %t0, LDA %t0 -> STA %t0 */
			if (match(list, STA, LDA, [&](BasicLinePtr a, BasicLinePtr b){

				if (a->opcode.addressMode() == b->opcode.addressMode()) {

					dp_register reg_a, reg_b;
					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {
							list.pop_front(); // a
							list.pop_front(); // b
							list.push_front(a);
							return true;
						}
					}
				}

				/*
				 * sta >_toolErr
				 * lda |_toolErr
				 */
				if (a->opcode.addressMode() == absolute_long && b->opcode.addressMode() == absolute) {

					identifier id_a, id_b;
					if (a->operands[0]->is_identifier(id_a) && b->operands[0]->is_identifier(id_b)) {
						//static identifier ToolError = nullptr;
						//if (!identifier) identifier = intern("_toolErr");
						// it shouldn't matter if it's toolErr or something else.
						if (id_a == id_b) {
							list.pop_front(); // a
							list.pop_front(); // b
							list.push_front(a);
							return true;
						}
					}

				}
				return false;
			})) continue;

			/* STA %t0, STA %t2, LDA %t0 -> STA, STA */
			if (match(list, STA, STA, LDA, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){

				// does b's address mode matter?
				if (a->opcode.addressMode() == c->opcode.addressMode()) {

					dp_register reg_a, reg_c;
					if (a->operands[0]->is_register(reg_a) && c->operands[0]->is_register(reg_c)) {
						if (reg_a == reg_c) {
							list.pop_front(); // a
							list.pop_front(); // b
							list.pop_front(); // c
							list.push_front(b);
							list.push_front(a);
							return true;
						}
					}
				}
				return false;
			})) continue;

			/* STA %t0, LDX %t0 -> STA %t0, TAX */
			if (match(list, STA, LDX, [&](BasicLinePtr a, BasicLinePtr b){
				//printf("STA %d/ LDA %d\n" , a->opcode.addressMode(), b->opcode.addressMode());

				if (a->opcode.addressMode() == b->opcode.addressMode()) {

					dp_register reg_a, reg_b;
					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {
							list.pop_front(); // a
							list.pop_front(); // b
							BasicLinePtr tmp = BasicLine::Make(TAX, implied);

							list.push_front(tmp);
							list.push_front(a);
							return true;
						}
					}
				}
				return false;
			})) continue;

			/* STA %t0, LDY %t0 -> STA %t0, TAY */
			if (match(list, STA, LDX, [&](BasicLinePtr a, BasicLinePtr b){

				if (a->opcode.addressMode() == b->opcode.addressMode()) {

					dp_register reg_a, reg_b;
					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {
							list.pop_front(); // a
							list.pop_front(); // b
							BasicLinePtr tmp = BasicLine::Make(TAY, implied);

							list.push_front(tmp);
							list.push_front(a);
							return true;
						}
					}
				}
				return false;
			})) continue;


			/* STA %t0, PEI (<%t0) -> STA %t0, PHA */
			if (match(list, STA/zp, PEI, [&](BasicLinePtr a, BasicLinePtr b){
				dp_register reg_a, reg_b;

				if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
					if (reg_a == reg_b) {

						BasicLinePtr tmp = BasicLine::Make(PHA, implied);

						list.pop_front(); // a
						list.pop_front(); // b
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

			/* STA %t0, STA %t2, PEI %t0 */
			if (match(list, STA/zp, STA, PEI, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){
				dp_register reg_a, reg_c;

				if (a->operands[0]->is_register(reg_a) && c->operands[0]->is_register(reg_c)) {
					if (reg_a == reg_c) {
						BasicLinePtr tmp = BasicLine::Make(PHA, implied);

						list.pop_front(); // a
						list.pop_front(); // b
						list.pop_front(); //c

						list.insert(list.begin(), {a, b, tmp});
						return true;
					}
				}
				return false;
			})) continue;


			/* STA %t0, PLY, LDA %t0  -> STA %t0, PLY */
			/* PLY is used to clean the stack after a cdecl call */
			/* doesn't preserve nz bits... */
			if (match(list, STA/zp, PLY, LDA/zp, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){
				dp_register reg_a, reg_c;

				if (a->operands[0]->is_register(reg_a) && c->operands[0]->is_register(reg_c)) {
					if (reg_a == reg_c) {

						list.pop_front(); // a
						list.pop_front(); // b
						list.pop_front(); //c
						list.insert(list.begin(), {a, b});
						return true;
					}
				}
				return false;
			})) continue;





			/* STA %t0, LDA xxx, CLC, ADC %t0 -> STA %t0, CLC, ADC xxx */
			// works for dp or immediate (or absolute...)
			if (match(list, STA/zp, LDA, CLC, ADC/zp, 
				[&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c, BasicLinePtr d){
					dp_register reg_a, reg_d;

					if (a->operands[0]->is_register(reg_a)
						&& d->operands[0]->is_register(reg_d)
					) {
						if (reg_a != reg_d) return false;

						list.pop_front();
						list.pop_front();
						list.pop_front();
						list.pop_front();

						BasicLinePtr tmp = BasicLine::Make(ADC, b->opcode.addressMode(), b->operands[0]);
						tmp->calc_registers();
						list.insert(list.begin(), {a, c, tmp});

						return true;
					}
					return false;

			})) continue;

			// sta %t0, rep #20, lda %t0
			// ... 


			// STX first to improve other optimizations.
			if (match(list, STA/zp, STX/zp, [&](BasicLinePtr a, BasicLinePtr b){
					dp_register reg_a, reg_b;

					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {

							// can drop the sta... unless m != x.
							list.pop_front();

						} else {
							list.pop_front();
							list.pop_front();

							list.insert(list.begin(), {b, a});
						}
						return true;
					}
					return false;

		 	})) continue;




			// STY first to improve other optimizations.
			if (match(list, STA/zp, STY/zp, [&](BasicLinePtr a, BasicLinePtr b){
					dp_register reg_a, reg_b;

					if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
						if (reg_a == reg_b) {

							// can drop the sta... unless m != x.
							list.pop_front();

						} else {
							list.pop_front();
							list.pop_front();

							list.insert(list.begin(), {b, a});
						}
						return true;
					}
					return false;
	
		 	})) continue;

			// STZ first to improve other optimizations.
			if (match(list, STA/zp, STZ/zp, [&](BasicLinePtr a, BasicLinePtr b){
					dp_register reg_a, reg_e;

					list.pop_front();
					list.pop_front();

					list.insert(list.begin(), {b, a});

					return true;

		 	})) continue;



			/*
				reading a byte:
				sep #$20
				lda ...
				sta <$01 \
				rep #$20 |
				lda <$01 |
				and #$ff /
				---
				sta <$01
				rep #$20
				and #$ff				
			 */

			if (match(list, STA/zp, REP, LDA/zp, AND/immediate, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c, BasicLinePtr d) {

				dp_register reg_a, reg_c;
				uint32_t int_b, int_d;
				if (a->operands[0]->is_register(reg_a) 
					&& b->operands[0]->is_integer(int_b) 
					&& c->operands[0]->is_register(reg_c)
					&& d->operands[0]->is_integer(int_d)) {

					if (reg_a == reg_c && (int_b & 0x20) && int_d <= 0xff) {

						list.pop_front();
						list.pop_front();
						list.pop_front();
						list.pop_front();

						list.insert(list.begin(), {
							a, b, d
						});

						return true;
					}

				}
				return false;

			})) continue;

			// sta %t0, lda %t2, eor %t0 -> sta %t0, eor %t2
			// also applies to AND, ORA.
			if (match(list, STA/zp, LDA/zp, (AND/zp)|(EOR/zp)|(ORA/zp), [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){
				dp_register reg_a, reg_b, reg_c;

				if (a->operands[0]->is_register(reg_a) 
					&& b->operands[0]->is_register(reg_b) 
					&& c->operands[0]->is_register(reg_c)) {

					if (reg_a == reg_c) {

						list.pop_front();
						list.pop_front();
						list.pop_front();

						BasicLinePtr tmp = BasicLine::Make(c->opcode.mnemonic(), zp, b->operands[0]);
						tmp->calc_registers();

						list.insert(list.begin(), {a, tmp});

						return true;
					}
				}
				return false;
			})) continue;

			break;


#if 0
	// commenting out... swap LDA, LDX and STA, STX to put x first, for better optimizations later.
			/*
			 * STA <$01
			 * STX <$03
			 * LDA <$05
			 * CLC 
			 * ADC <$01
			 * 
			 * (generated by casting int to long int, then adding)
			 *
			 * to
			 * STA <$01
			 * STX <$03
			 * CLC 
			 * ADC <$05
			 *
			 * better register analysis would catch this.
			 */
			 if (match(list, STA, STX, LDA, CLC, ADC,
				[&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c, BasicLinePtr d, BasicLinePtr e){
					dp_register reg_a, reg_e;

					if (a->opcode.addressMode() != zp) return false;
					if (e->opcode.addressMode() != zp) return false;

					if (a->operands[0]->is_register(reg_a)
						&& e->operands[0]->is_register(reg_e)
					) {
						if (reg_a != reg_e) return false;

						list.pop_front();
						list.pop_front();
						list.pop_front();
						list.pop_front();
						list.pop_front();

						BasicLinePtr tmp = BasicLine::Make(ADC, c->opcode.addressMode(), c->operands[0]);
						tmp->calc_registers();
						list.insert(list.begin(), {a, b, d, tmp});

						return true;

					}
					return false;

		 	})) continue;
#endif

			break;


		case STX:
			/* STX %t0 PEI %t0 -> STX, PHX */
			if (match(list, STX/zp, PEI, [&](BasicLinePtr a, BasicLinePtr b){
				dp_register reg_a, reg_b;
				if (a->operands[0]->is_register(reg_a) && b->operands[0]->is_register(reg_b)) {
					if (reg_a == reg_b) {

						list.pop_front();
						list.pop_front();

						BasicLinePtr tmp = BasicLine::Make(PHX, implied);

						list.insert(list.begin(), {a, tmp});

						return true;
					}
				}
				return false;
			})) continue;


			/* STX %t0, PHA, pei %t0 -> pha, phx, stx */
			/* makes dead-write elimination easier */
			if (match(list, STX/zp, PHA, PEI, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){
				dp_register reg_a, reg_c;
				if (a->operands[0]->is_register(reg_a) && c->operands[0]->is_register(reg_c)) {
					if (reg_a != reg_c) return false;


					list.pop_front();
					list.pop_front();
					list.pop_front();
					BasicLinePtr tmp = BasicLine::Make(PHX, implied);
					tmp->calc_registers();
					list.insert(list.begin(), {a, b, tmp});

					return true;
				}
				return false;
			})) continue;

			break;

		case TAX:
			/* TAX, PLX -> PLX */
			if (match(list, TAX, PLX, [&](BasicLinePtr a, BasicLinePtr b){

				list.pop_front(); // a

				return true;
			})) continue;	
			break;

		case TXA:
			/* TXA, LDA -> LDA */
			// todo -- long/short bits.
			if (match(list, TXA, LDA|PLA, [&](BasicLinePtr a, BasicLinePtr b){

				list.pop_front(); // a

				return true;
			})) continue;	

			/* TXA, STA -> STX ..., TXA */
			// todo -- long/short bits.
			if (match(list, TXA, STA/zp, [&](BasicLinePtr a, BasicLinePtr b){

				list.pop_front(); // a
				list.pop_front();
				BasicLinePtr tmp = BasicLine::Make(STX, b->opcode.addressMode(), b->operands[0]);
				tmp->calc_registers();
				list.insert(list.begin(), {tmp, a});

				return true;
			})) continue;


			break;


		case TYA:
			/* TYA, LDA -> LDA */
			// todo -- long/short bits.
			if (match(list, TYA, LDA|PLA, [&](BasicLinePtr a, BasicLinePtr b){

				list.pop_front(); // a

				return true;
			})) continue;	
			break;


#if 0
		case BRA:
			/* bra label, label: -> label */
			if (match(list, BRA, kUndefinedMnemonic, [&](BasicLinePtr a, BasicLinePtr b){
				identifier label;
				if (b->label && a->operands[0]->is_identifier(label)) {
					if (label == b->label) {
						list.pop_front();

						return true;
					}
				}
				return false;
			})) continue;
			break;

		case BRL:
			/* brl label, label: -> label */
			if (match(list, BRL, kUndefinedMnemonic, [&](BasicLinePtr a, BasicLinePtr b){
				identifier label;
				if (b->label && a->operands[0]->is_identifier(label)) {
					if (label == b->label) {
						list.pop_front();

						return true;
					}
				}
				return false;
			})) continue;
			break;

		case JMP:
			/* brl label, label: -> label */
			if (match(list, JMP, kUndefinedMnemonic, [&](BasicLinePtr a, BasicLinePtr b){
				identifier label;
				if (b->opcode.addressMode() == absolute 
					&& b->label 
					&& a->operands[0]->is_identifier(label)) {
					if (label == b->label) {
						list.pop_front();

						return true;
					}
				}
				return false;
			})) continue;
			break;
#endif

		}

		list.pop_front();
		optimized.push_back(line);
	}


	bool delta = (optimized.size() != starting_size);

	block->lines = std::move(optimized);
	if (recalc) block->recalc_next_set();

	return (delta|| recalc);
}



bool final_peephole(BasicBlockPtr block) {
	/*
	 * last-chance optimizations
	 *
	 */

	bool recalc = false;
	LineQueue list = std::move(block->lines);
	BasicLinePtr exit_branch = block->exit_branch;

	static identifier ToolError = nullptr;
	if (!ToolError) ToolError = intern("_toolErr");

	// hmmm can't just drop CMP #0 since CMP sets the c flag,
	// LDA does not.  CMP #0 bcs ... == if (x >= 0)
	// but bcs only useful for unsigned... shouldn't ever 
	// check against const #0 unsigned never < 0, unsigned always >= 0

	LineQueue optimized;

	uint32_t starting_size = list.size();


	while(!list.empty()) {

		BasicLinePtr line = list.front();

		OpCode opcode = line->opcode;
		switch(opcode.mnemonic()) {
		default: break;

		case JSL:
			// JSL, RTL -> JML
			if (match(list, JSL/absolute_long, RTL, [&](BasicLinePtr a, BasicLinePtr b){
				list.pop_front();
				list.pop_front();

				BasicLinePtr tmp = BasicLine::Make(JML, absolute_long,  a->operands[0]);
				tmp->calc_registers();

				list.push_front(tmp);

				return true;
			})) continue;

			/* jsl $e10000, sta >_toolErr, cmp #0, beq/bne -> */
			/* jsl $e10000, sta >_toolErr, bcc / bcs */
			if (match(list, JSL/absolute_long, STA/absolute_long, CMP/immediate, eof, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c) {
				uint32_t vector;
				identifier te;
				uint32_t zero;

				if (!exit_branch) return false;

				if (
					a->operands[0]->is_integer(vector) &&
					b->operands[0]->is_identifier(te) &&
					c->operands[0]->is_integer(zero)
				) {


					// tool, user tool, gsos vectors.
					if ((vector == 0xe10000 || vector == 0xe10008 || vector == 0xe100b0)
						&& te == ToolError
						&& zero == 0
					) {

						switch(exit_branch->branch.type) {
							case branch::eq:
								exit_branch->branch.type = branch::cc;
								break;
							case branch::ne:
								exit_branch->branch.type = branch::cs;
								break;

							default: 
								return false;
						}

						list.pop_front();
						list.pop_front();
						list.pop_front();

						list.insert(list.begin(), { a, b });

						return true;
					}

				}

				return false;
			})) continue;
			break;

		case JSR:

			// JSL, RTL -> JMP
			if (match(list, JSR/absolute, RTS, [&](BasicLinePtr a, BasicLinePtr b){
				list.pop_front();
				list.pop_front();

				BasicLinePtr tmp = BasicLine::Make(JMP, absolute,  a->operands[0]);
				tmp->calc_registers();

				list.push_front(tmp);

				return true;
			})) continue;

		// AND xxx, cmp #0
		// ORA xxx, cmp #0
		// EOR xxx, cmp #0
		// ASL, ROR, etc?

		case LDA:
			// lda 1,s ply rtl -> pla rtl
			if (match(list, LDA/stack_relative, PLY, RTL|RTS, [&](BasicLinePtr a, BasicLinePtr b, BasicLinePtr c){
				uint32_t int_a;
				if (a->operands[0]->is_integer(int_a)) {
					if (int_a == 1) {
						auto tmp = BasicLine::Make(PLA, implied);
						tmp->calc_registers();

						list.pop_front();
						list.pop_front();
						list.push_front(tmp);
						return true;
					}
				}
				return false;
			})) continue;

		case PLA:
		case TYA:
		case TXA:
		case TAY: // a == y.
		case TAX:
		case ORA:
		case EOR:
		case AND:
		case ADC:
		case SBC:
			// INC / DEC / ASL / LSR / ROL / ROR as well, but that shouldn't happen?
			/* LDA xxx, CMP #0, branch -> LDA xxx, branch */
			if (match(list, opcode.mnemonic(), CMP/immediate, eof, [&](BasicLinePtr a, BasicLinePtr b){
				uint32_t value;
				if (!exit_branch) return false;

				if (b->operands[0]->is_integer(value)) {
					if (value == 0 && !exit_branch->branch.reads_c() && !exit_branch->branch.reads_v()) {
						// drop the cmp
						list.pop_front();
						list.pop_front();
						list.push_front(a);

						return true;
					}
				}
				return false;
			})) continue;
			break;

		case TCD:
			if (match(list, TCD, TDC, [&](BasicLinePtr a, BasicLinePtr b){

				list.pop_front();
				list.pop_front();
				list.push_front(a);

				return true;
			})) continue;

#if 0
		case LDX:
			/* LDX xxx, CPX #0 -> LDX xxx */
			if (match(list, LDX, CPX, [&](BasicLinePtr a, BasicLinePtr b){
				uint32_t value;
				if (b->opcode.addressMode() == immediate && b->operands[0]->is_integer(value)) {
					if (value == 0) {
						list.pop_front();
						list.pop_front();
						list.push_front(a);

						return true;
					}
				}
				return false;
			})) continue;
			break;

		case LDY:
			/* LDY xxx, CPY #0 -> LDY xxx */
			if (match(list, LDY, CPY, [&](BasicLinePtr a, BasicLinePtr b){
				uint32_t value;
				if (b->opcode.addressMode() == immediate && b->operands[0]->is_integer(value)) {
					if (value == 0) {
						list.pop_front();
						list.pop_front();
						list.push_front(a);

						return true;
					}
				}
				return false;
			})) continue;
			break;
#endif

		}

		list.pop_front();
		optimized.emplace_back(std::move(line));
	}

	bool delta = (optimized.size() != starting_size);

	block->lines = std::move(optimized);
	if (recalc) block->recalc_next_set();

	return (delta|| recalc);
}
