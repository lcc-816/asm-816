

#include "common.h"
#include "Expression.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <cassert>

#include "cxx/defer.h"

typedef std::unordered_map<identifier, BasicBlockPtr> BlockMap;

namespace {
template<class T>
void remove_duplicates(std::vector<T> &t){
	std::sort(t.begin(), t.end());
	auto iter = std::unique(t.begin(), t.end());
	t.erase(iter, t.end());
}



template<class T>
void remove(std::vector<T> &v, T t){
	v.erase(std::remove(v.begin(), v.end(), t), v.end());
}

template<class T, class FX>
void remove_if(std::vector<T> &v, FX fx){
	v.erase(std::remove_if(v.begin(), v.end(), fx), v.end());
}

template<class T, class FX>
void remove_if(std::deque<T> &v, FX fx){
	v.erase(std::remove_if(v.begin(), v.end(), fx), v.end());
}

template<class T>
void replace(std::vector<T> &v, const T &old_value, const T &new_value){
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

static bool is_unconditional_branch(Mnemonic m)
{
	switch(m) {

		case JMP: // JMP (address,x) used for switch statement.
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

bool analyze_block_2(BasicBlockPtr block);



bool remove_branches(BlockQueue &bq) {
	// returns true if any changes made
	// returns false if no optimizations done.

	bool delta = false;
	for (auto iter = bq.begin(); iter != bq.end(); ++iter) {


		auto nextIter = iter + 1;
		if (nextIter == bq.end()) continue;

		BasicBlockPtr block = *iter;
		BasicBlockPtr next = *nextIter;


		if (block->dead) continue;
		if (block->lines.empty()) continue;

		BasicLinePtr line = block->lines.back();
		if (!is_branch(line->opcode.mnemonic(), false)) continue;
		// jmp (|abs,x) cannot be optimized out.


		if (block->next_set.size() != 1) continue;
		//if (next->prev_set.size() != 1) continue;

		// currently, must be the next in the queue... no re-ordering.

		if (block->next_set.front() != next) continue;
		//if (next->prev_set.front() != block) continue;

		block->lines.pop_back();
		delta = true;
	}

	return delta;
}


bool merge_blocks(BlockQueue &bq) {

	bool delta = false;


	// returns true if any changes made.

	for (BasicBlockPtr  block : bq) {

		if (block->dead) continue;
		if (block->next_set.size() != 1) continue;

		BasicBlockPtr next = block->next_set.front();
		if (next->prev_set.size() != 1) continue;

		assert(next->prev_set.front() == block);

		if (next->entry_node) continue; // can't merge.
		if (next->exit_node) block->exit_node = true;

		LineQueue &lines = block->lines;

		// remove branch...
		if (lines.size()) {
			BasicLinePtr line = lines.back();
			if (is_branch(line->opcode.mnemonic())) {
				lines.pop_back();
			}

			if (line->directive == SMART_BRANCH) {
				lines.pop_back();
			}
		}

		delta = true;
		if (lines.empty()) lines = std::move(next->lines);
		else lines.insert(lines.end(), next->lines.begin(), next->lines.end());


		// shouldn't need to rename anything since branch was removed.

		block->next_set = std::move(next->next_set);

		for (BasicBlockPtr newnext : block->next_set) {
			replace(newnext->prev_set, next, block);
		}

		next->dead = true;
		next->lines.clear();
		next->next_set.clear();
		next->prev_set.clear();


		// reconcile the import/export sets.
		// if next imports %t0 and block exports %t0,
		// next no longer needs to import it.
		register_set tmp = next->reg_import;
		tmp -= block->reg_export;
		block->reg_import += tmp;

		block->reg_export += next->reg_export;

		// and optimize them.
		for(;;) {
			bool delta = false;
			if (peephole(lines)) delta = true;
			if (analyze_block_2(block)) delta = true;

			if (!delta) break;
		}

	}


	return delta;
}

BlockQueue make_basic_blocks(LineQueue &&lines) {

	BlockQueue out;

	BasicBlockPtr current = nullptr;

	for (BasicLinePtr &line : lines) {


		Mnemonic m = line->opcode.mnemonic();

		if (line->label) {

			current = BasicBlock::Make();
			current->label = line->label;
			out.push_back(current);

			continue;
		}

		if (!current) {
			current = BasicBlock::Make();
			out.push_back(current);
		}

		current->lines.push_back(line);
		if (is_branch(m)) {
			current = nullptr;
		}
		if (line->directive == SMART_BRANCH) {
			current = nullptr;
		}
	}
	lines.clear();

	if (!out.empty()) out.front()->entry_node = true;
	if (!out.empty()) out.back()->exit_node = true;

	// check for drop-through
	for (auto iter = out.begin(); iter != out.end(); ++iter) {
		auto &block = *iter;
		auto next = iter + 1;
		bool fallthrough = false;

		if (next == out.end()) continue;
		
		if (block->lines.empty()) {
			fallthrough = true;
		} else { 
			BasicLinePtr& line = block->lines.back();
			Mnemonic m = line->opcode.mnemonic();
			if (is_conditional_branch(m))
				fallthrough = true;
			if (!is_branch(m))
				fallthrough = true;

			if (line->directive == SMART_BRANCH) {
				fallthrough = line->branch.is_conditional();
			}

		} 

		if (fallthrough)
			block->next_set.push_back(*next);

	}


	return out;
}



void analyze_block(BasicBlockPtr &block, const BlockMap &bm) {

	register_set reg_live;
	register_set reg_dead;
	register_set reg_import;
	register_set reg_export;

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

	auto next_set = std::move(block->next_set);

	// generate a list of next-blocks
	for (BasicLinePtr line : lines) {

		std::vector<identifier> ll;

		ExpressionPtr e = line->operands[0];

		if (is_branch(line->opcode.mnemonic())) {

			ll = e->identifiers();
		}

		// dc.l label is a reference.
		switch(line->directive)
		{
			case SMART_BRANCH:
				ll = e->identifiers();
				break;

			case DCB:
			case DCL:
			case DCW:
				ll = e->identifiers();
				break;
			default:
				break;
		}

		for (identifier label : ll) {
			auto iter = bm.find(label);
			if (iter != bm.end()) next_set.push_back(iter->second);
		}

	}
	// and remove duplicates.

	remove_duplicates(next_set);
	block->next_set = std::move(next_set);




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
	register_set reg_live; // = block->reg_import;
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

void dead_code_eliminate(BlockQueue &bq) {

	// need to scan multiple times
	// since removing one block can propogate
	// to others.
	bool delta = false;
	do {
		delta = false;
		for (BasicBlockPtr block : bq) {
			if (block->dead) continue;
			if (block->entry_node) continue;
			if (block->exit_node) continue; //

			if (block->prev_set.empty()) {

				// remove self from the next set.
				for (BasicBlockPtr next : block->next_set) {
					remove(next->prev_set, block);
				}

				delta = true;
				block->dead = true;

				block->prev_set.clear();
				block->next_set.clear();
				block->lines.clear();

				continue;
			}
		}

	} while (delta);

	remove_if(bq, [](BasicBlockPtr block) {
		return block->dead;
	});
}

static void build_imports(BasicBlockPtr block, register_set imports) {

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

void print_block_set(const std::vector<BasicBlockPtr> &set) {
	for (auto x : set) { if (x->label) printf("%s ", x->label->c_str()); }
	printf("\n");
}

void basic_block(Segment *segment) {

	BlockMap bm;

	BlockQueue bq = make_basic_blocks(std::move(segment->lines));


	auto kill_bq = make_defer([bq]{ 
		for (auto &b : bq) {
			b->prev_set.clear();
			b->next_set.clear();
		}
	});

	// create the map
	for (BasicBlockPtr &block : bq) {
		if (!block->label) continue;
		bm.emplace(block->label, block);
	}


	for (BasicBlockPtr &block : bq) {

		analyze_block(block, bm);
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


	dead_code_eliminate(bq);


	// propogate register imports.
	build_imports(bq);


	// now do a second lifetime scan to remove dead writes
	// across blocks.

	bool propagate_const(LineQueue &list);
	bool reg_const(LineQueue &list);

	for(;;) {
		bool any_delta = false;

		for (BasicBlockPtr  &block : bq) {
			// these probably don't need multiple passes...
			if (propagate_const(block->lines)) any_delta = true;
			if (reg_const(block->lines)) any_delta = true;

			for(;; any_delta = true) {
				bool delta = false;

				if (peephole(block->lines)) delta = true;
				if (analyze_block_2(block)) delta = true;
				//if (propagate_const(block->lines)) delta = true; // multipass?
				if (!delta) break;
				any_delta = true;

			}
		}


		// merge and remove blocks.
		for(;; any_delta = true) {

			bool delta = false;
			if (merge_blocks(bq)) delta = true;

			if (delta) {
				remove_if(bq, [](BasicBlockPtr block) {
					return block->dead;
				});
			}
			if (!delta) break;
		}

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
	bool final_peephole(LineQueue &list);


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

		if (
			!lines.empty()
			&& lines.back()->directive == SMART_BRANCH
			&& lines.back()->branch.type == branch::always
			&& block->next_set.size() == 1 
			&& block->next_set.front()->exit_node
			&& block->next_set.front()->lines.empty()
			&& segment->epilogue_code.size() < 3) {

			lines.pop_back();
			block->exit_node = true;
		}

		if (block->exit_node) {
			lines.insert(lines.end(), 
				segment->epilogue_code.begin(), 
				segment->epilogue_code.end()
			);
		}



		final_peephole(lines); // remove extraneous cmp #0, etc.

		for (auto &line : lines) {
			switch(line->directive) {
				case SMART_BRANCH:
					{
						auto tmp = line->branch.to_code(line->operands[0]);
						out.insert(out.end(), tmp.begin(), tmp.end());
					}
					break;
				case PROLOGUE:
					//out.insert(out.end(), segment->prologue_code.begin(), segment->prologue_code.end());
					break;
				case EPILOGUE:
					//out.insert(out.end(), segment->epilogue_code.begin(), segment->epilogue_code.end());
					break;

				default:
					out.push_back(line);
					break;
			}
		}

		//out.insert(out.end(), block->lines.begin(), block->lines.end());
		block->lines.clear();
	}
	
	// optimize JSL address / RTL to JML address.
	// now handled by peephole.
#if 0
	if (out.size() && segment->epilogue_code.size() == 1) {
		BasicLinePtr back = out.back();
		BasicLinePtr tmp = segment->epilogue_code.back();

		if (back->opcode.mnemonic() == JSL && back->opcode.addressMode() == absolute_long) {
			if (tmp->opcode.mnemonic() == RTL) {
				back->opcode = OpCode(m65816, JML, absolute_long);
				segment->epilogue_code.clear();
			}
		}

		if (back->opcode.mnemonic() == JSR && back->opcode.addressMode() == absolute) {
			if (tmp->opcode.mnemonic() == RTS) {
				back->opcode = OpCode(m65816, JMP, absolute);
				segment->epilogue_code.clear();
			}
		}

	}
#endif

	//out.insert(out.end(), segment->epilogue_code.begin(), segment->epilogue_code.end());

	// need to clear the next/previous sets to break the memory retain cycles.
	#if 0
	for (auto &block : bq) {
		block->next_set.clear();
		block->prev_set.clear();
	}
	#endif
	segment->lines = std::move(out);
}

