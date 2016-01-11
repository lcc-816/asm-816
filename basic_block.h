#ifndef __basic_block_h__
#define __basic_block_h__

#include "types.h"
#include "common.h"
#include "dp_register_set.h"
#include "register_set.h"

#include <memory>

class BasicBlock : public std::enable_shared_from_this<BasicBlock> {

public:

	static BlockQueue MakeBlockQueue(LineQueue &&);

	template<class...Args>
	static BasicBlockPtr Make(Args&&... args) {
		return std::make_shared<BasicBlock>(std::forward<Args>(args)...);
	}

	void recalc_next_set();
	void make_dead();

	void remove_prev(BasicBlockPtr);
	void remove_next(BasicBlockPtr);
	void replace_prev(BasicBlockPtr oldBlock, BasicBlockPtr newBlock);
	void replace_next(BasicBlockPtr oldBlock, BasicBlockPtr newBlock);

	void dump() const;

	unsigned pc = 0;


	identifier label = nullptr;

	LineQueue lines;
	BasicLinePtr exit_branch; // if ends w/ a branch.
	BasicBlockPtr next_block; // if fallthrough.

	std::vector<BasicBlockPtr> next_set;
	std::vector<BasicBlockPtr> prev_set;


	dp_register_set dp_reg_import;
	dp_register_set dp_reg_export;

	register_set reg_import;
	register_set reg_export;

	bool processed = false;
	bool dead = false; // mark for dead-code elimination.
	bool dirty = false; // if optimization has modified it!


	bool entry_node = false; // this is an entry point (can't dead-code eliminate)
	bool exit_node = false;

};

#endif
