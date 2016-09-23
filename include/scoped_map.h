#ifndef __scoped_map_h__
#define __scoped_map_h__

#include <vector>
#include <unordered_map>

template<class K, class V>
class scoped_map {
	
public:

	scoped_map() {
		_spine.reserve(2);
		_spine.emplace_back();		
	}

	scoped_map(scoped_map &&) = default;
	scoped_map(const scoped_map &) = default;

	scoped_map& operator=(scoped_map &&) = default;
	scoped_map& operator=(const scoped_map &) = default;


	V find(K key, bool current_scope_only = false) {
		if (current_scope_only) {
			auto iter = _spine.back().find(key);
			return iter == _spine.back().end() ? V(): iter->second;
		}
		for (auto iter = _spine.rbegin(); iter != _spine.rend(); ++iter) {
			auto kv = iter->find(key);
			if (kv != iter->end()) return kv->second;
		}

		return V();
	}

	// returns true if inserted, false if duplicate key.
	bool insert(K key, V value) {
		auto p = _spine.back().insert(std::make_pair(key, value));
		return p.second;
	}

	void push() {
		_spine.emplace_back();
	}
	void pop() {
		if (_spine.size() > 1) _spine.pop_back();
	}

private:
	std::vector< std::unordered_map<K, V> > _spine;
};

#endif
