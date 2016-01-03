#include "types.h"
#include "common.h"

#include "cxx/tsv2_deque.h"

/*
 * remove dead blocks, ie, blocks which 
 * have no previous set and are not an entry node.
 *
 */
bool dead_block_elimination(BlockQueue &bq) {

	// need to scan multiple times
	// since removing one block can propogate
	// to others.

	unsigned size = bq.size();
	bool delta = false;
	for(;;) {

		unsigned size = bq.size();
		erase_if(bq, [](BasicBlockPtr block) {
			if (block->dead) return true;
			if (block->entry_node) return false;
			//if (block->exit_node) return false;

			if (block->prev_set.empty()) {
				block->make_dead();
				return true;
			}
			return false;
		});

		if (size == bq.size()) break;
	}


	if (size != bq.size()) {
		return true;
	}
	return false;
}
