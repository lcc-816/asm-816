
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


void add_imports(BasicBlockPtr block, register_set imports) {
	imports -= block->reg_export;
	if (!imports) return;

	if (block->reg_import.contains(imports)) return;

	block->reg_import += imports;
	for (auto prev : block->prev_set) 
		add_imports(prev, block->reg_import);
}


void import_export(BasicBlockPtr block) {
	
	register_set reg_import;
	register_set reg_export;

	// if (block->label && *block->label == ".404") {
	// 	printf("here\n");
	// }

	for (BasicLinePtr line : block->lines) {

		OpCode opcode = line->opcode;
		line->calc_registers();

		auto rs = line->read_registers();
		auto ws = line->write_registers();
		if (!rs.zp() && !ws.zp()) continue;

		dp_register reg = line->reg;

		// if it reads a dp register, mark it as imported...
		// unless it was previously written by this block. 
		// (in which case it will be in the export set.
		if (rs.zp()) {
			for_each(line->reg, line->reg_count, [&reg_import, &reg_export](dp_register r){
				if (!reg_export.contains(r)) reg_import += r;
			});
			//reg_import += (registers - reg_export)
		}

		// if it writes to a dp register, mark it as exported.
		if (ws.zp()) {
			for_each(line->reg, line->reg_count, [&reg_export](dp_register r) { reg_export += r; });
			//reg_export += registers;
		}

	}

	block->reg_import = reg_import;
	block->reg_export = reg_export;


	// if (block->label && *block->label == ".404") {
	// 	printf("imports: ");
	// 	block->reg_import.dump();
	// 	printf("exports: ");
	// 	block->reg_import.dump();
	// }

}


void build_import_export_set(BlockQueue &bq) {

	for (auto block : bq) {
		import_export(block);
	}

	// now propogate the imports.
	for (auto block : bq) {
		const auto &imports = block->reg_import;
		if (!imports) continue;

		for (auto prev : block->prev_set) {
			add_imports(prev, imports);
		} 

	}

	// now remove unnecessary exports!
	for (auto block : bq) {
		register_set exports;
		for (auto next : block->next_set) {
			exports += next->reg_import;
		}

		block->reg_export &= exports;
	}
}


//struct register_pair { dp_register base, unsigned size = 0};

bool dataflow_analysis(BasicBlockPtr block) {

	auto size = block->lines.size();
	LineQueue tmp;
	register_set live = block->reg_export;



	std::copy_if(block->lines.rbegin(), block->lines.rend(), std::front_inserter(tmp), [&live, block](BasicLinePtr line){

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

				if (none_of(line->reg, line->reg_count, [&live](dp_register r){
					return live.contains(r);
				})) return false;
				//if (!live.contains(rp)) return false;
			}

			for_each(line->reg, line->reg_count, [&live](dp_register r){
				live -= r;
			});

			//live -= rp;
		}
		if (rs.zp()) {
			// reading from a zp -- mark as live.
			for_each(line->reg, line->reg_count, [&live](dp_register r){
				live += r;
			});
			//live += rp;
		}

		return true;
	});




	if (tmp.size() == size) return false;
	block->lines = std::move(tmp);
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
