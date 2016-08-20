

#include "common.h"
#include "Expression.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>

#include <cassert>

#include "cxx/defer.h"
#include "cxx/tsv2_vector.h"
#include "cxx/tsv2_deque.h"

bool common_line_consolidation(BasicBlockPtr block);
bool register_lifetime(BasicBlockPtr block);

bool dead_block_elimination(BlockQueue &bq);
bool dataflow_analysis(BlockQueue &);
bool dataflow_analysis(BasicBlockPtr);
bool remove_branches(BlockQueue &bq);
bool merge_blocks(BlockQueue &bq);


namespace {

	std::unordered_map<identifier, BasicBlockPtr> BlockMap;


template<class T, class A>
void remove_duplicates(std::vector<T, A> &t){
	std::sort(t.begin(), t.end());
	auto iter = std::unique(t.begin(), t.end());
	t.erase(iter, t.end());
}


template <class T, class A>
void replace(std::vector<T, A> &v, const T &old_value, const T &new_value){
	std::replace(v.begin(), v.end(), old_value, new_value);
}

template <class T, class A>
void replace(std::deque<T, A> &v, const T &old_value, const T &new_value){
	std::replace(v.begin(), v.end(), old_value, new_value);
}


} // namespace



static bool is_branch(Mnemonic m, bool include_jmp = true)
{
	switch(m) {

		/*
		case BBR0:
		case BBR1:
		case BBR2:
		case BBR3:
		case BBR4:
		case BBR5:
		case BBR6:
		case BBR7:
		case BBS0:
		case BBS1:
		case BBS2:
		case BBS3:
		case BBS4:
		case BBS5:
		case BBS6:
		case BBS7:
		*/
		case JMP: // JMP (address,x) used for switch statement.
			if (include_jmp) return true;
			return false;

		case JML:
		case BCC:
		case BCS:
		case BVC:
		case BVS:
		case BMI:
		case BPL:
		case BEQ:
		case BNE:
		case BRA:
		case BRL:
			return true;
		default:
			return false;
	}
}

static bool is_unconditional_branch(Mnemonic m, bool include_jmp = true)
{
	switch(m) {

		case JMP: // JMP (address,x) used for switch statement.
			if (include_jmp) return true;
			return false;

		case JML:
		case BRA:
		case BRL:
			return true;
		default:
			return false;
	}	
}

static bool is_conditional_branch(Mnemonic m) {
	switch(m) {
		case BCC:
		case BCS:
		case BVC:
		case BVS:
		case BMI:
		case BPL:
		case BEQ:
		case BNE:
			return true;
		default:
			return false;		
	}
}




void BasicBlock::dump() const {
	printf ("BasicBlock %p\n", this);
	if (label) printf("label: %s\n", label->c_str());
	if (dead) printf("*dead*\n");
	printf("lines: %d\n", (int)lines.size());
	printf("next_set: %d\n", (int)next_set.size());
	printf("prev_set: %d\n", (int)prev_set.size());
	printf("\n");
}

void BasicBlock::make_dead() {
	dead = true;

	auto self = shared_from_this();
	for (auto b : prev_set) {
		b->remove_next(self);
	}

	for (auto b : next_set) {
		b->remove_prev(self);
	}

	if (label) BlockMap.erase(label);

	lines.clear();
	next_set.clear();
	prev_set.clear();
	next_block = nullptr;
	exit_branch = nullptr;
	label = nullptr;
}


void BasicBlock::remove_prev(BasicBlockPtr target) {
	if (!target) return;
	erase(prev_set, target);
}

void BasicBlock::remove_next(BasicBlockPtr target) {
	if (!target) return;
	if (next_block == target) next_block = nullptr;
	erase(next_set, target);
}

void BasicBlock::replace_prev(BasicBlockPtr oldBlock, BasicBlockPtr newBlock) {
	replace(prev_set, oldBlock, newBlock);
	remove_duplicates(prev_set);
}

void BasicBlock::replace_next(BasicBlockPtr oldBlock, BasicBlockPtr newBlock) {
	if (next_block == oldBlock) next_block = newBlock;
	// also fix branches?
	replace(next_set, oldBlock, newBlock);
	remove_duplicates(next_set);
}

void BasicBlock::recalc_next_set() {
	std::vector<identifier> labels;

	std::vector<BasicBlockPtr> new_next_set;
	if (next_block) new_next_set.push_back(next_block);



	const BasicLinePtr& line = exit_branch;
	if (line) {
		ExpressionPtr e = line->operands[0];
		labels = e->identifiers();
	}

	// generate a list of next-blocks
	for (BasicLinePtr &line : lines) {

		// dc.l label is a reference.
		switch(line->directive)
		{
			case DCB:
			case DCL:
			case DCW:
			{
				ExpressionPtr e = line->operands[0];
				auto tmp = e->identifiers();
				labels.insert(labels.end(), tmp.begin(), tmp.end());
				break;
			}
			default:
				break;
		}
	}
	// and remove duplicates.
	remove_duplicates(labels);

	for (identifier label : labels) {
		auto iter = BlockMap.find(label);
		if (iter != BlockMap.end()) new_next_set.push_back(iter->second);
	}

	remove_duplicates(new_next_set);

	// if any entries in current next set not in the new next set,
	// remove their back-link to this block.

	std::vector<BasicBlockPtr> diff;
	std::set_difference(
		next_set.begin(), next_set.end(), 
		new_next_set.begin(), new_next_set.end(),
		std::back_inserter(diff));


	auto self = shared_from_this();
	for (auto block : diff) {
		block->remove_prev(self);
	}

	next_set = std::move(new_next_set);
}





BlockQueue make_basic_blocks(LineQueue &&lines) {

	BlockQueue out;

	BasicBlockPtr current = nullptr;
	BasicBlockPtr prev = nullptr;
	bool fallthrough = false;

	for (BasicLinePtr &line : lines) {


		Mnemonic m = line->opcode.mnemonic();

		if (line->label) {

			// if there is currently a block, fallthrough to the new one.
			if (current) {
				prev = current;
				fallthrough = true;
			}

			current = BasicBlock::Make();
			current->label = line->label;

			if (line->global) current->global_node = true;
			out.push_back(current);

			if (fallthrough && prev) {
				prev->next_set.push_back(current);
				prev->next_block = current;
			}
			fallthrough = false;
			prev = nullptr;

			continue;
		}

		if (!current) {
			current = BasicBlock::Make();
			out.push_back(current);

			if (fallthrough && prev) {
				prev->next_set.push_back(current);
				prev->next_block = current;
			}

			fallthrough = false;
			prev = nullptr;
		}

		if (is_branch(m)) {
			current->exit_branch = line;
			prev = current;
			current = nullptr;

			fallthrough = is_conditional_branch(m);
			continue;
		}

		if (line->directive == SMART_BRANCH) {
			current->exit_branch = line;
			prev = current;
			current = nullptr;

			fallthrough = line->branch.is_conditional();
			continue;
		}

		current->lines.push_back(line);
	}
	lines.clear();

	if (!out.empty()) {
		out.front()->entry_node = true;
		out.back()->exit_node = true;
	}

	return out;
}



void print_block_set(const std::vector<BasicBlockPtr> &set) {
	for (auto x : set) { if (x->label) printf("%s ", x->label->c_str()); }
	printf("\n");
}

void basic_block(Segment *segment) {

	BlockQueue bq = make_basic_blocks(std::move(segment->lines));

	auto kill_bm = make_defer([]{
		BlockMap.clear();
	});

	auto kill_bq = make_defer([bq]{ 
		for (auto &b : bq) {
			b->prev_set.clear();
			b->next_set.clear();
			b->next_block = nullptr;
		}
	});

	// create the map
	BlockMap.clear();
	BlockMap.reserve(bq.size());

	for (BasicBlockPtr &block : bq) {
		if (!block->label) continue;
		BlockMap.emplace(block->label, block);
	}

	for (BasicBlockPtr &block : bq) {

		block->recalc_next_set();
	}

	// if the block falls through, add the next block 

	// generate prev-blocks
	for (BasicBlockPtr &block : bq) {
		for (BasicBlockPtr next : block->next_set) {

			next->prev_set.push_back(block);
		}
	}

	// and unique them.
	for (BasicBlockPtr &block : bq)
		remove_duplicates(block->prev_set);



	dataflow_analysis(bq);

	for (BasicBlockPtr & block : bq)
		common_line_consolidation(block);

	dead_block_elimination(bq);


	// propogate register imports.
	//build_imports(bq);


	// now do a second lifetime scan to remove dead writes
	// across blocks.

	bool propagate_const(LineQueue &list);
	bool reg_const(LineQueue &list);

	for(;;) {
		int any_delta = 0;

		for (BasicBlockPtr  &block : bq) {
			// these probably don't need multiple passes...
			if (propagate_const(block->lines)) any_delta = true;
			if (reg_const(block->lines)) any_delta = true;
			if (register_lifetime(block)) any_delta = true;

			for(;; any_delta = true) {
				int delta = 0;

				delta += (peephole(block));
				//if (analyze_block_2(block)) delta = true;
				//if (propagate_const(block->lines)) delta = true; // multipass?
				if (!delta) break;
				any_delta = true;
			}
		}


		// merge and remove blocks.
		for(;; any_delta = true) {

			bool delta = false;
			if (merge_blocks(bq)) delta = true;

			if (!delta) break;
		}

		// re-calc imports/exports if anything else changed.
		if (any_delta) dataflow_analysis(bq);

		// remove any redundant branches.
		remove_branches(bq);
		if (!any_delta) break;
	}

	// todo -- remove trailing branch to next block
	// if only 1 exit.
	// if is_branch(list.back() && == next)..

	// todo -- if next_set.size() == 1 && next_set->prev_set.size() == 1
	// merge the blocks together and re-optimize.

	void fix_branches(Segment *segment, BlockQueue &blocks);
	void assign_registers(Segment *segment, BlockQueue &);


	assign_registers(segment, bq); // creates epilogue/prologue.
	fix_branches(segment, bq);


	//
	LineQueue out;

	//out.insert(out.end(), segment->prologue_code.begin(), segment->prologue_code.end());

	for (BasicBlockPtr &block : bq) {

		if (block->dead) continue;

		auto &lines = block->lines;

		if (block->entry_node) {
			// insert it here so it can be peepholed.
			lines.insert(lines.begin(), 
				segment->prologue_code.begin(), 
				segment->prologue_code.end()
			);
		}

		if (block->label) {
			auto line = BasicLine::Make(block->label);
			if (block->global_node) line->global = true;
			lines.emplace_front(std::move(line));
		}

		// if this branches to an exit node and the exit code is small, just do it here.
		// can't be too big or it could screw up branching.

		auto exit_branch = block->exit_branch;
		if (exit_branch
			&& exit_branch->directive == SMART_BRANCH
			&& exit_branch->branch.type == branch::always
			&& block->next_set.size() == 1 
			&& block->next_set.front()->exit_node
			&& block->next_set.front()->lines.empty()
			&& segment->epilogue_code.size() < 3
			&& segment->convention != Segment::naked) {

			block->exit_branch = nullptr;
			block->exit_node = true;
			exit_branch = nullptr;
		}

		if (block->exit_node) {
			lines.insert(lines.end(), 
				segment->epilogue_code.begin(), 
				segment->epilogue_code.end()
			);
		}



		final_peephole(block); // remove extraneous cmp #0, etc.
		exit_branch = block->exit_branch;

		//std::copy(lines.begin(), lines.end(), std::back_inserter(out));
		//out.insert(out.end(), lines.begin(), lines.end());
		for (auto &line : lines) out.emplace_back(std::move(line));

		if (exit_branch) {
			if (exit_branch->directive == SMART_BRANCH) {
				auto tmp = exit_branch->branch.to_code(exit_branch->operands[0]);
				out.insert(out.end(), tmp.begin(), tmp.end());
			} else {
				out.push_back(exit_branch);
			}
		}

		//out.insert(out.end(), block->lines.begin(), block->lines.end());
		block->lines.clear();
	}
	

	segment->lines = std::move(out);
}

