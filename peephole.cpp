
#include <array>

#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"


template<class FX>
static bool match(Line *head, Mnemonic m1, FX fx)
{
	if (!head || head->instruction.mnemonic() != m1) return false;

	const unsigned Size = 1;
	Line *lines[Size] = {};
	Mnemonic mm[Size] = { m1 };

	for(unsigned i = 0; i < Size; ++i) {
		if (!head) return false;
		if (head->instruction.mnemonic() != mm[i]) return false;
		lines[i] = head;
		head = head->next;
	}

	return fx(lines);
}

template<class FX>
static bool match(Line *head, Mnemonic m1, Mnemonic m2, FX fx)
{
	if (!head || head->instruction.mnemonic() != m1) return false;

	const unsigned Size = 2;
	Line *lines[Size] = {};
	Mnemonic mm[Size] = { m1, m2 };

	for(unsigned i = 0; i < Size; ++i) {
		if (!head) return false;
		if (head->instruction.mnemonic() != mm[i]) return false;
		lines[i] = head;
		head = head->next;
	}

	return fx(lines);
}

template<class FX>
static bool match(Line *head, Mnemonic m1, Mnemonic m2, Mnemonic m3, FX fx)
{
	if (!head || head->instruction.mnemonic() != m1) return false;

	const unsigned Size = 3;
	Line *lines[Size] = {};
	Mnemonic mm[Size] = { m1, m2, m3 };

	for(unsigned i = 0; i < Size; ++i) {
		if (!head) return false;
		if (head->instruction.mnemonic() != mm[i]) return false;
		lines[i] = head;
		head = head->next;
	}

	return fx(lines);
}

Line *peephole(Line *head) {

	Line *prev = nullptr;
	Line *newHead = nullptr;

	Line *line = head;
	while (line) {

		/* PHA, PLA -> NOP */
		if (match(line, PHA, PLA, [&line, &prev](Line *lines[2]){
			// remove both.

			line = lines[1]->next;
			if (prev) prev->next = line;

			delete lines[0];
			delete lines[1];
			return true;
		})) continue;

		/* PEA xxx, PLA -> LDA #xxx */
		if (match(line, PEA, PLA, [&line](Line *lines[2]){

			line->instruction = Instruction(m65816, LDA);
			line->mode = immediate;
			line->explicit_mode = true;

			line->next = lines[1]->next;
			delete lines[1];
			return true;
		})) continue;

		/* PEI (xxx), PLA -> LDA <xxx */
		if (match(line, PEI, PLA, [&line](Line *lines[2]){

			line->instruction = Instruction(m65816, LDA);
			line->mode = zp;
			line->explicit_mode = true;

			line->next = lines[1]->next;
			delete lines[1];
			return true;
		})) continue;

		/* LDA #const, XBA -> lda #const-reversed */
		if (match(line, LDA, XBA, [&line](Line *lines[2]){

			uint32_t value;
			if (line->mode == immediate && line->operands[0]->is_integer(value)) {

				value &= 0xffff;
				line->operands[0] = Expression::Integer((value >> 8) | (value << 8));

				line->next = lines[1]->next;
				delete lines[1];
				return true;
			}
			return false;
		})) continue;

		/* STA address, LDA address -> STA */
		if (match(line, STA, LDA, [&line](Line *lines[2]){

			dp_register a,b;
			if (lines[0]->mode == lines[1]->mode 
				&& lines[0]->operands[0]->is_register(a)
				&& lines[1]->operands[0]->is_register(b)
			) {
				if (a == b) {
					line->next = lines[1]->next;
					delete lines[1];
					return true;
				}
			}
			return false;

		})) continue;

		// BRA label, label:  -> nop
		// kUndefinedMnemonic is a label
		if (match(line, BRA, kUndefinedMnemonic, [&line, &prev](Line *lines[2]){
			const std::string *a, *b;
			b = lines[1]->label;
			if (lines[0]->operands[0]->is_variable(a)) {
				if (a == b) {
					if (prev) prev->next = line->next;
					delete line;
					line = lines[1];
					return true;
				}
			}
			return false;
		})) continue;

		if (match(line, BRL, kUndefinedMnemonic, [&line, &prev](Line *lines[2]){
			const std::string *a, *b;
			b = lines[1]->label;
			if (lines[0]->operands[0]->is_variable(a)) {
				if (a == b) {
					if (prev) prev->next = line->next;
					delete line;
					line = lines[1];
					return true;
				}
			}
			return false;
		})) continue;

		if (match(line, JMP, kUndefinedMnemonic, [&line, &prev](Line *lines[2]){
			const std::string *a, *b;
			b = lines[1]->label;
			if (lines[0]->mode == absolute && lines[0]->operands[0]->is_variable(a)) {
				if (a == b) {
					if (prev) prev->next = line->next;
					delete line;
					line = lines[1];
					return true;
				}
			}
			return false;
		})) continue;


		if (!newHead) newHead = line;
		prev = line;
		line = line->next;
	}

	return newHead;
}

/*

enum {
	match_fail,
	match_nop,
	match_delete,
} match_type;

// helper function that checks return type?

template<unsigned Size, class FX>
std::enable_if< std::is_same<Line *, std::result_of<fx(Line *[Size])>::type > >::type
invoke(std::deque<Line *> &dq, Lines *lines[Size]) {
	Line *line = fx(lines);
	if (line) {
		for (unsigned i = 0; i < Size; ++i) {
			delete dq.front();
			dq.pop_front();
		}
		dq.push_front(line);
		return true;
	}
	return false;
}

template<unsigned Size, class FX>
std::enable_if< std::is_same<match_type, std::result_of<fx(Line *[Size])>::type > >::type
invoke(std::deque<Line *> &dq, Lines *lines[Size]) {
	match_type mt = fx(lines);
	switch (mt)
	{
	case match_nop:
		return true;
	case match_fail:
		return false;
	case match_delete:
		for (unsigned i = 0; i < Size; ++i) {
			delete dq.front();
			dq.pop_front();
		}
		return true;	
	}
}

bool match(std::deque<Line> &dq, Mnemonic m1, Mnemonic m2, FX fx){
	const Size = 2;
	if (lines.size() < 2) return false;

	Lines *lines[Size] = {};
	Mnemonic mm[Size] = { m1, m2};

	for (unsigned i = 0; i < Size; ++i) {
		lines[i] = dq[i];
		if (lines[i]->instruction.mnemonic() != mm[i]) return false;
	}

	auto st = fx(dq, lines);
	if (st == match_nop) return true;
	if (st == match_delete) {
		for (unsigned i = 0; i < Size; ++i) {
			delete lines[i];
			dq.pop_front();
		}
		return true;
	}

	if (st == match_fail) return false;
	return false;
}

std::deque<Line> peephole(std::deque<Line> dq) {
	
	std::deque<Line> out;

	while(!dq.empty()) {


		if (match(dq, PHA, PLA [&lines](){
			return match_delete;
		})) continue;

		if (match(line, PEA, PLA, [](Line *a, Line *b){

			Line *ll = new Line;
			ll->instruction = Instruction(m65816, LDA);
			ll->operands[0] = a->operands[0];
			ll->address_mode = immediate;
			ll->explicit_mode = true;
			return ll;
		})) continue;

		if (match(line, PEI, PLA, [](Line *lines[2]){
			Line *ll = new Line;
			//...
			return ll;
		})) continue;


		Line line = dq.front();
		dq.pop_front();
		out.push_back(line);
	}

	return out;
}

*/

