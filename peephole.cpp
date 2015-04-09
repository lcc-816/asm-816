
#include <array>
#include <deque>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"
#include "OpCode.h"

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


void peephole(LineQueue &list) {

	LineQueue optimized;

	while(!list.empty()) {

		BasicLine *line = list.front();

		switch(line->opcode.mnemonic()) {
		default: break;

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
			break;

		case PEA:
			/* PEA xxx, PLA -> LDA #xxx */
			if (match(list, PEA, PLA, [&](BasicLine *a, BasicLine *b){

				BasicLine *tmp = new BasicLine;
				tmp->operands[0] = a->operands[0];
				tmp->opcode = OpCode(Instruction(m65816, LDA), immediate);

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

				BasicLine *tmp = new BasicLine;
				tmp->operands[0] = a->operands[0];
				tmp->opcode = OpCode(Instruction(m65816, LDA), zp);

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


		case STA:
			/* STA %t0, LDA %t0 -> STA %t0 */
			if (match(list, STA, LDA, [&](BasicLine *a, BasicLine *b){
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
			break;

		case BRA:
			/* bra label, label: -> label */
			if (match(list, BRA, kUndefinedMnemonic, [&](BasicLine *a, BasicLine *b){
				const std::string *label;
				if (b->label && a->operands[0]->is_variable(label)) {
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
				if (b->label && a->operands[0]->is_variable(label)) {
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
					&& a->operands[0]->is_variable(label)) {
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
}