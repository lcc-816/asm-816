#include "types.h"
#include "common.h"
#include "Expression.h"


#include <algorithm>
#include <numeric>
#include <cassert>

bool dead_block_elimination(BlockQueue &bq);

BasicBlockPtr next_valid_block(BasicBlockPtr block) {

	while (block) {
		if (block->entry_node) break;
		if (block->exit_node) break;
		if (block->global_node) break;
		if (block->dead) return BasicBlockPtr();

		if (block->lines.size() || block->exit_branch) break;

		block = block->next_block;
	}
	return block;
}

bool merge_blocks(BasicBlockPtr &block, BasicBlockPtr &next) {
	if (!block || !next) return false;
	if (block->dead || next->dead) return false;

	if (next->entry_node) return false;
	if (next->global_node) return false;
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
	block->dp_reg_export = std::move(next->dp_reg_export);
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

	// update the next_block to skip past any empty next blocks.
	for (auto block : bq) {
		auto next_block = block->next_block;
		if (!next_block) continue;
		auto new_next_block = next_valid_block(next_block);

		if (new_next_block != next_block) {
			block->replace_next(next_block, new_next_block);
			next_block->remove_prev(block);
			delta = true;
		}
	}


	// unconditional branches don't have the fall-through next_block.
	// this will remove the exit branch and add the next_block.
	std::accumulate(bq.begin(), bq.end(), BasicBlockPtr(), [&delta](BasicBlockPtr block, BasicBlockPtr next_block){
		if (!block || block->dead) return next_block;
		if (!next_block || next_block->dead) return next_block; // ?

		if (block->next_set.size() != 1 || block->next_set.front() != next_block) return next_block;

		auto &exit_branch = block->exit_branch;
		if (!exit_branch || exit_branch->opcode.mnemonic() == JMP) return next_block;

		// also merge if possible?

		block->exit_branch = nullptr;
		block->next_block = next_block;

		delta++;
		block->dirty = true;

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
				block->dirty = true;
				continue; // retry with next block!
			}
		}
		++iter;
	}

	if (delta) dead_block_elimination(bq);
	return delta;
}

#if 0
bool fix_fallthrough(BlockQueue &bq) {
	bool delta = false;

	for (auto block : bq) {
		if (!block->fallthrough) continue;
		auto oldf = block->fallthrough;
		auto newf = next_valid_block(oldf);
		if (oldf == newf) continue;
		delta = true;
		block->replace_next(oldf, newf);
	}

	return delta;
}
#endif

#if 0

1.
	beq 3
	fallthrough = 2
	next = [2, 3]
2.	[empty]
3. 
	...

	if conditional branch, next_valid_block(fallthrough) == branch target
	remove branch, remove empty block.

	merge blocks ... if empty, no exit, merge into _next_ block (keep next block label, etc)

bool remove_empty_block(BlockQueue &bq) {

	bool delta = false;

	for (auto block : bq) {
		if (bq->dead) continue;
		if (bq->lines.size()) continue;
		if (bq->exit_node) continue;
		if (bq->entry_node) continue;
		if (bq->global_node) continue;

		auto exit_branch = block->exit_branch;

		// update all previous nodes to point to the next node
		if (!exit_branch) {
			auto self = shared_from_this();
			auto next = block->next_set.front();
			assert(block->next_set.size() == 1);

			for (auto prev : block->prev_set) {
				prev->replace_next(self, next);

			}

			block->make_dead();
			delta = true;
			continue;
		} 

	}
	if (delta) dead_block_elimination(bq);
	return delta;
}
#endif

