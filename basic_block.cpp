

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
	erase(next_set, target);
}

void BasicBlock::replace_prev(BasicBlockPtr oldBlock, BasicBlockPtr newBlock) {
	replace(prev_set, oldBlock, newBlock);
}

void BasicBlock::replace_next(BasicBlockPtr oldBlock, BasicBlockPtr newBlock) {
	replace(next_set, oldBlock, newBlock);
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

#if 0
	// check for drop-through
	for (auto iter = out.begin(); iter != out.end(); ++iter) {
		auto &block = *iter;
		auto next = iter + 1;
		bool fallthrough = false;

		if (next == out.end()) continue;
		

		BasicLinePtr &line = block->exit_branch;
		if (line) {

			Mnemonic m = line->opcode.mnemonic();
			if (is_conditional_branch(m))
				fallthrough = true;

			if (line->directive == SMART_BRANCH) {
				fallthrough = line->branch.is_conditional();


		} else {
			fallthrough = true;
		}


		if (fallthrough)
			block->next_set.push_back(*next);

	}
#endif

	return out;
}


#if 0
void analyze_block(BasicBlockPtr &block) {

	dp_register_set reg_live;
	dp_register_set reg_dead;
	dp_register_set reg_import;
	dp_register_set reg_export;

	LineQueue lines = std::move(block->lines);

	std::vector<BasicBlockPtr> next;

	// scan forward to calculate lifetimes.
	for (BasicLinePtr &line : lines) {

		line->reg_live = reg_live;

		line->calc_registers();
		const dp_register reg = line->reg;
		auto status = line->reg_status;

		if (status == reg_read || status == reg_rw) {
			dp_register r = reg;
			// need to break it out here since we only
			// import a reg if it's not live.
			for (unsigned i = 0; i < line->reg_count; ++i) {

				if (!reg_live.contains(r))
					reg_import += r;

				r.number += 2;
			}

		}


		if (status == reg_write || status == reg_rw) {

			reg_export.insert(reg, line->reg_count);
		}

		reg_live.insert(reg, line->reg_count);
	}


	// imports are live in all lines.
	if (reg_import) {
		for (BasicLinePtr line : block->lines) {
			line->reg_live += reg_import;
		}
	}

	// scan backwards and remove dead writes.
	// todo -- still needed or will analyze_block_2 catch it?
	/*
	 *  sta %t0 << can be killed
	 *  ...
	 *  ...
	 *  sta %t0
	 */

	LineQueue newLines;
	while(!lines.empty()) {

		BasicLinePtr line = lines.back();
		lines.pop_back();

		bool dead = false;


		line->reg_live -= reg_dead;

		const dp_register reg = line->reg;
		unsigned reg_count = line->reg_count;

		if (reg && reg.is_temporary())
		switch (line->reg_status) {

			case reg_none:
				break;

			case reg_read:
			case reg_rw:
				reg_dead.remove(reg, reg_count);
				line->reg_live.insert(reg, reg_count);
				break;

			case reg_write:
				dp_register r = reg;
				// can only delete if all registers are dead.
				if (reg_dead.contains(reg, reg_count))
					dead = true;
				reg_dead.insert(reg, reg_count);
				break;
		}


		if (dead) {
			//printf("deleting %s\n", line->opcode.toString());
		}
		else newLines.push_front(line);
	}

	block->lines = std::move(newLines);
	block->reg_import = reg_import;
	block->reg_export = reg_export;
}

bool analyze_block_2(BasicBlockPtr block) {

	// second round of analysis.  import set has been calculated at this point.

	LineQueue newLines;
	LineQueue lines = std::move(block->lines);
	dp_register_set reg_live; // = block->reg_import;
	//reg_live += block->reg_export;

	bool delta = false;

	// only need to export registers imported by next blocks.
	for (BasicBlockPtr block : block->next_set) {
		reg_live += block->reg_import;
	}


	while(!lines.empty()) {

		BasicLinePtr line = lines.back();
		lines.pop_back();

		bool dead = false;

		dp_register reg = line->reg;
		unsigned reg_count = line->reg_count;

		if (reg && reg.is_temporary())
		switch (line->reg_status) {

			case reg_none:
				break;

			case reg_read:
				reg_live.insert(reg, reg_count);
				break;

			case reg_rw:
				// read, then write.
				// if reg is live, it remains live.
				// if reg is dead, it remains dead.
				if (!reg_live.contains_any(reg, reg_count))
					dead = true;

				break;				


			case reg_write:
				// drop the write if not live.
				if (!reg_live.contains_any(reg, reg_count))
					dead = true;
				reg_live.remove(reg, reg_count); // any reason not to?
				// ugh... what if it's a cross-byte write?
				// sta.w %t1 write %t1 and %t2, but this treats it as %t0, %t1, %t2, %t3.  
				// need to treat as bytes, not words.
				break;
		}


		if (dead) {
			//printf("deleting %s\n", line->opcode.toString());
			delta = true;
		}
		//if (dead) { delete line; delta = true; }
		else newLines.push_front(line);
	}


	block->lines = std::move(newLines);
	return delta;

}


static void build_imports(BasicBlockPtr block, dp_register_set imports) {

	imports -= block->reg_export;
	if (block->processed && block->reg_import.contains(imports)) return;

	block->processed = true;
	block->reg_import += imports;

	for (BasicBlockPtr prev : block->prev_set) {
		build_imports(prev, block->reg_import);
	}

}

static void build_imports(BlockQueue &bq) {

	// do it backwards since that should save time...
	for(auto iter = bq.rbegin(); iter != bq.rend(); ++iter) {
		BasicBlockPtr block = *iter;
		for (BasicBlockPtr prev : block->prev_set) {
			build_imports(prev, block->reg_import);
		}

	}

}
#endif

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

	void fix_branches(BlockQueue &blocks);
	void assign_registers(Segment *segment, BlockQueue &);


	assign_registers(segment, bq); // creates epilogue/prologue.
	fix_branches(bq);


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
			lines.emplace_front(BasicLine::Make(block->label));
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

