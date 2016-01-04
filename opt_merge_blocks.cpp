#include "types.h"
#include "common.h"
#include "Expression.h"


#include <algorithm>
#include <numeric>
#include <cassert>

bool dead_block_elimination(BlockQueue &bq);


bool merge_blocks(BasicBlockPtr &block, BasicBlockPtr &next) {
	if (!block || !next) return false;
	if (block->dead || next->dead) return false;

	if (next->entry_node) return false;
	if (next->prev_set.size() != 1) return false;

	assert(next->prev_set.front() == block);

	auto &exit_branch = block->exit_branch;
	if (exit_branch && exit_branch->opcode.mnemonic() == JMP) return false; // jmp (,x) table.

	LineQueue &lines = block->lines;

	if (next->exit_node) block->exit_node = true;
	if (lines.empty()) lines = std::move(next->lines);
	else lines.insert(lines.end(), next->lines.begin(), next->lines.end());

	block->exit_branch = std::move(next->exit_branch);
	block->next_block = std::move(next->next_block);
	block->reg_export = std::move(next->reg_export);
	block->next_set = std::move(next->next_set);

	for (BasicBlockPtr newnext : block->next_set) {
		newnext->replace_prev(next, block);
		//replace(newnext->prev_set, next, block);
	}

	next->make_dead();
	return true;
}



/*
   if bra/brl to fallthrough node, remove the branch.
   probably only needs to be done at the very end.

       lda #0
       bra foo
   foo
       ...



       lda #0
   foo
       ...
 */
bool remove_branches(BlockQueue &bq) {

	unsigned delta = 0;

	// unconditional branches don't have the fall-through next_block.
	// this will remove the exit branch and add the next_block.
	std::accumulate(bq.begin(), bq.end(), BasicBlockPtr(), [&delta](BasicBlockPtr block, BasicBlockPtr next_block){
		if (!block || block->dead) return next_block;
		if (!next_block || next_block->dead) return next_block; // ?

		if (block->next_set.size() || block->next_set.front() != next_block) return next_block;

		auto &exit_branch = block->exit_branch;
		if (!exit_branch || exit_branch->opcode.mnemonic() == JMP) return next_block;

		// also merge if possible?

		block->exit_branch = nullptr;
		block->next_block = next_block;

		delta++;

		if (merge_blocks(block, next_block)) return block;

		return next_block;
	});

	if (delta) dead_block_elimination(bq);
	return delta;
}

bool merge_blocks(BlockQueue &bq) {

	bool delta = false;


	// returns true if any changes made.
	for (auto iter = bq.begin(); iter != bq.end(); ) {
		auto block = *iter;
		if (block && block->next_set.size() == 1) {
			auto next = block->next_set.front();
			if (merge_blocks(block, next)) {
				delta = true;
				continue; // retry with next block!
			}
		}
		++iter;
	}

	if (delta) dead_block_elimination(bq);
	return delta;
}
