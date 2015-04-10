

#include "common.h"
#include "Expression.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

typedef std::unordered_map<const std::string *, BasicBlock *> BlockMap;

template<class T>
void remove_duplicates(std::vector<T> &t){
	std::sort(t.begin(), t.end());
	auto iter = std::unique(t.begin(), t.end());
	t.erase(iter, t.end());
}

static bool is_branch(Mnemonic m)
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

static bool unconditional_branch(Mnemonic m)
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




BlockQueue make_basic_blocks(LineQueue &&lines) {

	BlockQueue out;

	BasicBlock *current = nullptr;
	for (BasicLine *line : lines) {

		if (line->label) {
			// but not if it's a @local label...

			BasicBlock *old = current;

			current = new BasicBlock;
			current->label = line->label;


			// close the previous
			if (old) {
				out.push_back(old);

				bool fallthrough = true;
				if (!old->lines.empty()) {
					BasicLine *line = old->lines.back();

					// if it's a branch, don't fall through.
					if (unconditional_branch(line->opcode.mnemonic())) fallthrough = false;
				}
				if (fallthrough) old->next_set.push_back(current);

			} 

			//continue;

			// non-data directives should already be removed at this point.
		}

		if (!current) current = new BasicBlock;

		current->lines.push_back(line);
	}
	lines.clear();

	if (current) out.push_back(current);

	if (!out.empty()) out.front()->entry = true;
	return out;
}



void analyze_block(BasicBlock *block, const BlockMap &bm) {

	register_set reg_live;
	register_set reg_dead;
	register_set reg_import;
	register_set reg_export;

	LineQueue lines = std::move(block->lines);

	std::vector<BasicBlock *> next;

	// scan forward to calculate lifetimes.
	for (BasicLine *line : lines) {

		line->reg_live = reg_live;

		dp_register reg;
		if (!line->opcode) continue;

		if (line->opcode.addressMode() == implied) continue;
		if (!line->operands[0]) continue;
		if (!line->operands[0]->is_register(reg)) continue;

		line->reg = reg;
		int status = line->reg_status = classify(line->opcode);

		if (status == reg_read_long) { // register pair.

			dp_register reg2 = reg + 2;

			if (!reg_live.contains(reg2))
				reg_import += reg2;

			reg_live += reg2;
			status = reg_read; // handle below.
		}

		if (status == reg_read || status == reg_rw) {
			if (!reg_live.contains(reg))
				reg_import += reg;
		}
		if (status == reg_write || status == reg_rw) {
			reg_export += reg;
		}

		reg_live += reg;
	}

	auto next_set = std::move(block->next_set);

	// generate a list of next-blocks
	for (BasicLine *line : lines) {

		std::vector<const std::string *> ll;

		Expression *e = line->operands[0];

		if (is_branch(line->opcode.mnemonic())) {

			ll = e->collect_variables();
		}

		// dc.l label is a reference.
		switch(line->directive)
		{
			case DCB:
			case DCL:
			case DCW:
				ll = e->collect_variables();
				break;
			default:
				break;
		}

		for (const std::string *label : ll) {
			auto iter = bm.find(label);
			if (iter != bm.end()) next_set.push_back(iter->second);
		}

	}
	// and remove duplicates.

	remove_duplicates(next_set);
	block->next_set = std::move(next_set);




	// imports are live in all lines.
	if (reg_import) {
		for (BasicLine *line : block->lines) {
			line->reg_live += reg_import;
		}
	}

	// scan backwards and remove dead writes.
	/*
	 *  sta %t0 << can be killed
	 *  ...
	 *  ...
	 *  sta %t0
	 */

	LineQueue newLines;
	while(!lines.empty()) {

		BasicLine *line = lines.back();
		lines.pop_back();

		bool dead = false;


		line->reg_live -= reg_dead;

		dp_register reg = line->reg;

		switch (line->reg_status) {

			case reg_none:
				break;


			case reg_read_long:
				{
					dp_register reg2 = reg + 2;
					reg_dead -= reg2;
					line->reg_live += reg2;
				}
				// fallthrough

			case reg_read:
			case reg_rw:
				reg_dead -= reg;
				line->reg_live += reg;
				break;

			case reg_write:
				if (reg_dead.contains(reg))
					dead = true;
				reg_dead += reg;
				break;
		}


		if (dead) delete line;
		else newLines.push_front(line);
	}

	block->lines = std::move(newLines);
	block->reg_import = reg_import;
	block->reg_export = reg_export;
}

void analyze_block_2(BasicBlock *block) {

	// second round of analysis.  import set has been calculated at this point.

	LineQueue newLines;
	LineQueue lines = std::move(block->lines);
	register_set reg_live = block->reg_import;
	//reg_live += block->reg_export;


	// only need to export registers imported by next blocks.
	for (BasicBlock *block : block->next_set) {
		reg_live += block->reg_import;
	}


	while(!lines.empty()) {

		BasicLine *line = lines.back();
		lines.pop_back();

		bool dead = false;

		dp_register reg = line->reg;

		switch (line->reg_status) {

			case reg_none:
				break;

			case reg_read_long:
				{
					dp_register reg2 = reg + 2;
					reg_live += reg;
				}
				// fallthrough

			case reg_read:
			case reg_rw:
				reg_live += reg;
				break;

			case reg_write:
				// drop the write if not live.
				if (!reg_live.contains(reg))
					dead = true;
				break;
		}


		if (dead) delete line;
		else newLines.push_front(line);
	}
	block->lines = std::move(newLines);

}

template<class T>
static void remove_from_vector(T t, std::vector<T> &v){

	v.erase(std::remove(v.begin(), v.end(), t), v.end());

}

void dead_code_eliminate(BlockQueue &bq) {

	// need to scan multiple times
	// since removing one block can propogate
	// to others.
	bool delta = false;
	do {
		delta = false;
		for (BasicBlock *block : bq) {
			if (block->entry) continue;
			if (block->dead) continue;
			if (block->prev_set.empty()) {
				delta = true;
				block->dead = true;

				for (BasicLine *line : block->lines) delete line;
				block->lines.clear();

				// remove self from the next set.
				for (BasicBlock *next : block->next_set) {
					remove_from_vector(block, next->prev_set);
				}
			}
		}

	} while (delta);

	bq.erase(std::remove_if(bq.begin(), bq.end(), [](BasicBlock *block) {
		return block->dead;
	}), bq.end());
}

static void build_imports(BasicBlock *block, register_set imports) {

	imports -= block->reg_export;
	if (block->processed && block->reg_import.contains(imports)) return;

	block->processed = true;
	block->reg_import += imports;

	for (BasicBlock *prev : block->prev_set) {
		build_imports(prev, block->reg_import);
	}

}

static void build_imports(BlockQueue &bq) {

	// do it backwards since that should save time...
	for(auto iter = bq.rbegin(); iter != bq.rend(); ++iter) {
		BasicBlock *block = *iter;
		for (BasicBlock *prev : block->prev_set) {
			build_imports(prev, block->reg_import);
		}

	}

}

LineQueue basic_block(LineQueue &&lines) {

	BlockMap bm;

	BlockQueue bq = make_basic_blocks(std::move(lines));


	// create the map
	for (BasicBlock *block : bq) {
		if (!block->label) continue; // shouldn't happen, unless first one?
		bm.emplace(block->label, block);
	}


	for (BasicBlock *block : bq) {

		analyze_block(block, bm);
	}

	// if the block falls through, add the next block 

	// generate prev-blocks
	for (BasicBlock *block : bq) {
		for (BasicBlock *next : block->next_set) {

			next->prev_set.push_back(block);
		}
	}
	// and unique them.
	for (BasicBlock *block : bq)
		remove_duplicates(block->prev_set);

	dead_code_eliminate(bq);

	// propogate register imports.
	build_imports(bq);

	// now do a second lifetime scan to remove dead writes
	// across blocks.

	for (BasicBlock *block : bq) {
		analyze_block_2(block);
	}


	LineQueue out;

	for (BasicBlock *block : bq) {

		for (BasicLine *line : block->lines) {
			out.push_back(line);
		}
		block->lines.clear();

	}

	return out;
}

