#include "types.h"
#include "common.h"

#include <algorithm>
#include "Expression.h"
#include "branch.h"


/*
	Before:
.1
	...
	LDA #0
	BRA .3
.2 
	...
	LDA #0
	BRA .3
.3
	...

	After:

.1
	...
	BRA .3
.2 
	...
	BRA .3
.3
	LDA #0
	...


Only possible if lines are identical in all previous blocks.

 */

bool operator==(BasicLine & a, BasicLine &b)
{
	if (&a == &b) return true;

	if (a.label != b.label) return false;
	if (a.directive != b.directive) return false;
	if (a.opcode != b.opcode) return false;
	if (a.branch != b.branch) return false;


	return std::equal(
		std::begin(a.operands), std::end(a.operands), 
		std::begin(b.operands), std::end(b.operands), 
		[](ExpressionPtr a, ExpressionPtr b){
			if (a && b) return *a == *b;
			return a == b;
	});
}

// common line consolidation.
bool common_line_consolidation(BasicBlockPtr block) {
	

	// if all previous entries flow into this one (and only) this one,
	// move any common trailing lines.

	if (!block || block->dead || block->entry_node) return false;

	auto &prev_set = block->prev_set;

	if (prev_set.size() < 2) return false;

	if (std::any_of(prev_set.begin(), prev_set.end(), [](BasicBlockPtr b){
		return b->next_set.size() != 1; // also check if exit branch is null or always....
	})) return false;


	BasicBlockPtr first = prev_set.front();

	bool delta = false;
	while(!first->lines.empty()) {
		bool ok;
		BasicLinePtr target = first->lines.back();

		ok = std::all_of(prev_set.begin(), prev_set.end(), [target](BasicBlockPtr b){

			if (b->lines.empty()) return false;
			BasicLinePtr l = b->lines.back();
			if (l == target || *l == *target) return true;
			return false;
		});


		if (!ok) break;


		// now move it...
		for (auto &b : prev_set) { b->lines.pop_back(); }

		// need to reset reginfo?
		block->lines.emplace_front(std::move(target));

		delta = true;
	}

	return delta;
}


