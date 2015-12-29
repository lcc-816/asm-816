#include "symbol_table.h"

symbol_table::symbol_table() {
	_spine.reserve(2);
	_spine.emplace_back();
}

void symbol_table::pop() {
	if (_spine.size() > 1) {
		_spine.pop_back();
	}
}

void symbol_table::push() {
	_spine.emplace_back();
}


void symbol_table::insert(identifier key, ExpressionPtr value) {
	_spine.back().insert(std::make_pair(key, value));
}

ExpressionPtr symbol_table::find(identifier key) {
	
	for (auto iter = _spine.rbegin(); iter != _spine.rend(); ++iter) {
		auto kv = iter->find(key);
		if (kv != iter->end()) return kv->second;
	}

	return nullptr;
}