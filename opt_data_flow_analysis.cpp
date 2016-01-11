
/* data flow analysis */

#include "types.h"
#include "common.h"
#include "Expression.h"

#include "cxx/tsv2_deque.h"

template<class UnaryFunction>
void for_each(dp_register r, unsigned reg_count, UnaryFunction fx) {

	for (unsigned i = 0; i < reg_count; ++i) {
		fx(r);
		r.number += 2;
	}
}


template<class UnaryPredicate>
bool all_of(dp_register r, unsigned reg_count, UnaryPredicate fx) {

	for (unsigned i = 0; i < reg_count; ++i) {
		if (!fx(r)) return false;
		r.number += 2;
	}
	return true;
}

template<class UnaryPredicate>
bool none_of(dp_register r, unsigned reg_count, UnaryPredicate fx) {

	for (unsigned i = 0; i < reg_count; ++i) {
		if (fx(r)) return false;
		r.number += 2;
	}
	return true;
}

template<class UnaryPredicate>
bool any_of(dp_register r, unsigned reg_count, UnaryPredicate fx) {

	for (unsigned i = 0; i < reg_count; ++i) {
		if (fx(r)) return true;
		r.number += 2;
	}
	return false;
}


void add_imports(BasicBlockPtr block, dp_register_set imports) {
	imports -= block->dp_reg_export;
	if (!imports) return;

	if (block->dp_reg_import.contains(imports)) return;

	block->dp_reg_import += imports;
	for (auto prev : block->prev_set) 
		add_imports(prev, block->dp_reg_import);
}


void add_imports(BasicBlockPtr block, register_set imports) {
	imports -= block->reg_export;
	if (!imports) return;

	if (block->reg_import.includes(imports)) return;

	block->reg_import += imports;
	for (auto prev : block->prev_set) 
		add_imports(prev, block->reg_import);
}


void import_export(BasicBlockPtr block) {
	
	dp_register_set dp_reg_import;
	dp_register_set dp_reg_export;

	register_set reg_import;
	register_set reg_export;


	// if (block->label && *block->label == ".404") {
	// 	printf("here\n");
	// }

	// exit nodes also need to export a/x (if cdecl and not void...)
	#if 0
	if (block->exit_node) {
		reg_export += register_set(0x07); // magic!
		reg_import += register_set(0x07); // magic!
	}
	#endif

	for (BasicLinePtr line : block->lines) {

		OpCode opcode = line->opcode;
		line->calc_registers();

		auto rs = line->read_registers();
		auto ws = line->write_registers();

		reg_import += (rs - reg_export);
		reg_export += ws;



		if (!rs.zp() && !ws.zp()) continue;

		dp_register reg = line->reg;

		// if it reads a dp register, mark it as imported...
		// unless it was previously written by this block. 
		// (in which case it will be in the export set.
		if (rs.zp()) {
			for_each(line->reg, line->reg_count, [&dp_reg_import, &dp_reg_export](dp_register r){
				if (!dp_reg_export.contains(r)) dp_reg_import += r;
			});
			//dp_reg_import += (registers - dp_reg_export)
		}

		// if it writes to a dp register, mark it as exported.
		if (ws.zp()) {
			for_each(line->reg, line->reg_count, [&dp_reg_export](dp_register r) { dp_reg_export += r; });
			//dp_reg_export += registers;
		}

	}

	// also need to add in the branch block.
	BasicLinePtr &line = block->exit_branch;
	if (line) {

		OpCode opcode = line->opcode;
		line->calc_registers();

		auto rs = line->read_registers();
		auto ws = line->write_registers();

		reg_import += (rs - reg_export);
		reg_export += ws;

		// currently, branches don't read dp_registers.
	}


	block->dp_reg_import = dp_reg_import;
	block->dp_reg_export = dp_reg_export;

	block->reg_import = reg_import;
	block->reg_export = reg_export;



	// if (block->label && *block->label == ".404") {
	// 	printf("imports: ");
	// 	block->dp_reg_import.dump();
	// 	printf("exports: ");
	// 	block->dp_reg_import.dump();
	// }

}



void build_import_export_set(BlockQueue &bq) {

	for (auto block : bq) {
		import_export(block);

		if (block->exit_node) {
			block->reg_import += register_set(0x07); // export a, x, y
			block->reg_export += register_set(0x07); // export a, x, y
		}

	}

	// now propogate the imports.
	for (auto block : bq) {
		const auto &dp_imports = block->dp_reg_import;
		auto imports = block->reg_import;

		if (!dp_imports && !imports) continue;

		for (auto prev : block->prev_set) {
			add_imports(prev, dp_imports);
			add_imports(prev, imports);
		} 

	}

	// now remove unnecessary exports!
	for (auto block : bq) {
		dp_register_set dp_exports;
		register_set exports;

		for (auto next : block->next_set) {
			dp_exports += next->dp_reg_import;
			exports += next->reg_import;
		}

		if (block->exit_node) {
			exports += register_set(0x07);
		}

		block->dp_reg_export &= dp_exports;
		block->reg_export &= exports;

	}
}


//struct register_pair { dp_register base, unsigned size = 0};

bool dataflow_analysis(BasicBlockPtr block) {

	auto size = block->lines.size();
	LineQueue tmp;
	dp_register_set dp_live = block->dp_reg_export;

	std::copy_if(block->lines.rbegin(), block->lines.rend(), std::front_inserter(tmp), [&dp_live, block](BasicLinePtr line){

		OpCode opcode = line->opcode;
		if (!opcode) return true;

		auto rs = line->read_registers();
		auto ws = line->write_registers();

		dp_register reg = line->reg;

		//if (block->label && *block->label == ".404" && opcode.mnemonic() == STA) {
		//	printf(".\n");
		//}

		if (ws.zp()) {

			if (ws.p() == 0 && reg.is_temporary()) {

				if (none_of(line->reg, line->reg_count, [&dp_live](dp_register r){
					return dp_live.contains(r);
				})) return false;
				//if (!dp_live.contains(rp)) return false;
			}

			for_each(line->reg, line->reg_count, [&dp_live](dp_register r){
				dp_live -= r;
			});

			//dp_live -= rp;
		}
		if (rs.zp()) {
			// reading from a zp -- mark as live.
			for_each(line->reg, line->reg_count, [&dp_live](dp_register r){
				dp_live += r;
			});
			//dp_live += rp;
		}

		return true;
	});




	if (tmp.size() == size) return false;
	block->lines = std::move(tmp);
	block->dirty = true;
	return true;
}


bool dataflow_analysis(BlockQueue &bq) {

	unsigned any_delta = 0;
	for(;;) {

		unsigned delta = 0;
		build_import_export_set(bq);
		for (auto block : bq) {
			delta += dataflow_analysis(block);
		}
		any_delta += delta;
		if (!delta) break;
	}

	return any_delta;
}
