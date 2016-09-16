

#include "dp_register_set.h"

#include <stdexcept>
#include "include/tsv2_vector.h"

namespace {

	constexpr unsigned IndexCount = 4;

	inline int reg_to_index(char type) {
		switch (type) {
			case 't': return 0;
			case 'r': return 1;
			case 'v': return 2;
			case 'p': return 3;
			default:
				return -1;
		}
	}

	inline int reg_to_index(dp_register r) {
		return reg_to_index(r.type);
	}

	inline char index_to_reg(int ix) {
		switch(ix) {
			case 0: return 't';
			case 1: return 'r';
			case 2: return 'v';
			case 3: return 'p';
			default: return 0;
		}
	}
}
#if 0
template<class T, class BinaryFunction>
void with(T &lhs, const T &rhs, BinaryFunction b) {
	static_assert(lhs.size() == rhs.size(), "oops");
	for (unsigned i = 0; i < lhs.size(); ++i)
		b(lhs[i], rhs[i]);
}
#endif

#if 0
struct dp_register_pair {
	dp_register r;
	unsigned count;
};


struct dp_register_iterator : public std::iterator<std::bidirectional_iterator_tag, dp_register> {

	dp_register operator*() { return r; }

	bool operator==(const dp_register_iterator &rhs) { return r == rhs.r; }
	bool operator!=(const dp_register_iterator &rhs) { return r != rhs.r; }

	dp_register_iterator &operator++() {
		r.number += 2;
		return *this;
	}

	dp_register r;
};

dp_register_iterator begin(const dp_register_pair &pair) {
	return pair.r;
}

dp_register_iterator end(const dp_register_pair &pair) {
	return pair.r + pair.count * 2;
}
#endif


dp_register_set::dp_register_set(std::initializer_list<dp_register> l) {
	for (auto r : l) {
		int ix = reg_to_index(r);
		if (r.number < _data[ix].size()) { _data[ix][r.number] = true; }
		else {
			if (!_extra) _extra = std::vector<dp_register>();
			_extra->push_back(r);
		}
	}
	if (_extra) {
		std::sort(_extra->begin(), _extra->end());
		_extra->erase( std::unique(_extra->begin(), _extra->end()), _extra->end());
	}
}

dp_register_set::dp_register_set(const std::vector<dp_register> &v) {
	for (auto r : v) {
		int ix = reg_to_index(r);
		if (r.number < _data[ix].size()) { _data[ix][r.number] = true; }
		else {
			if (!_extra) _extra = std::vector<dp_register>();
			_extra->push_back(r);
		}
	}
	if (_extra) {
		std::sort(_extra->begin(), _extra->end());
		_extra->erase( std::unique(_extra->begin(), _extra->end()), _extra->end());
	}
}

bool dp_register_set::includes(dp_register r) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;

	if (_data[ix].size() > r.number) return _data[ix][r.number]; 

	if (_extra) return std::binary_search(_extra->begin(), _extra->end(), r);

	return false;

}

bool dp_register_set::includes(dp_register r, unsigned count) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;
	if (!count) return false;

	while (count && r.number < _data[ix].size()) {
		if (!_data[ix][r.number]) return false;
		r.number += 2;
		count--;
	}

	if (!count) return true;
	if (_extra) {
		auto iter = std::lower_bound(_extra->begin(), _extra->end(), r);
		if (iter == _extra->end()) return false;

		dp_register begin = r;
		dp_register end = r + count;

		// todo....		
		//return std::includes(iter, _extra->end(), begin, end);
	}

	return false;
}

bool dp_register_set::includes_any(dp_register r, unsigned count) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;
	if (!count) return false;

	while (count && r.number < _data[ix].size()) {

		if (_data[ix][r.number]) return true;
		r.number += 2;
		count--;
	}


	if (!count) return false;
	if (_extra && count) {
		auto iter = std::lower_bound(_extra->begin(), _extra->end(), r);
		if (iter == _extra->end()) return false;
		// iter is >= r.
		if (*iter == r) return true;
		if (iter->type == r.type && iter->number < r.number + count * 2) return true;
	}

	// todo -- extra

	return false;
}


bool dp_register_set::includes(const dp_register_set &rhs) {
	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		auto tmp = _data[ix];
		tmp.flip();
		tmp &= rhs._data[ix];
		if (tmp.count()) return false;
	}

	if (_extra && rhs._extra) {
		return std::includes(_extra->begin(), _extra->end(), rhs._extra->begin(), rhs._extra->end());		
	}

	if (rhs._extra && ! _extra) return false;

	return true;
}

dp_register_set &dp_register_set::operator += (dp_register r) {
	int ix = reg_to_index(r);
	if (ix < 0) return *this;


	if (_data[ix].size() < r.number) {
		//throw std::runtime_error("too many registers (bitset exceeded)");

		if (_extra) {
			auto iter = std::lower_bound(_extra->begin(), _extra->end(), r);

			// insert inserts *before* the iterator.
			if (iter == _extra->end() || *iter != r)
				_extra->insert(iter, r);
		}
		else {
			_extra = std::vector<dp_register>({r});
		}
		return *this;
	}

	_data[ix][r.number] = true;
	return *this;
}


void dp_register_set::insert(dp_register r, unsigned count) {
	unsigned i;
	for (i = 0; i < count; ++i) {
		*this += r;
		r.number += 2;
	}
}

void dp_register_set::remove(dp_register r, unsigned count) {
	unsigned i;
	for (i = 0; i < count; ++i) {
		*this -= r;
		r.number += 2;
	}
}



dp_register_set &dp_register_set::operator += (const dp_register_set &rhs) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {

		_data[ix] |= rhs._data[ix];
	}

	if (rhs._extra) {
		if (!_extra) _extra = rhs._extra;
		else {
			std::vector<dp_register> tmp;
			std::merge(
				_extra->begin(), _extra->end(),
				rhs._extra->begin(), rhs._extra->end(),
				std::back_inserter(tmp)
			);
			_extra = std::move(tmp);
		}
	}

	return *this;
}

dp_register_set &dp_register_set::operator -= (dp_register r) {

	int ix = reg_to_index(r);
	if (ix < 0) return *this;

	if (_data[ix].size() > r.number)
		_data[ix][r.number] = false;

	if (_extra) erase(*_extra, r);

	normalize_extra();
	return *this;	
}


dp_register_set &dp_register_set::operator -= (const dp_register_set &r) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		auto tmp = r._data[ix];
		tmp.flip();
		_data[ix] &= tmp;
	}

	if (_extra && r._extra) {
		std::vector<dp_register> tmp;
		std::set_difference(
			_extra->begin(), _extra->end(), 
			r._extra->begin(), r._extra->end(),
			std::back_inserter(tmp)
		);
		_extra = std::move(tmp);
	}

	normalize_extra();
	return *this;
}

dp_register_set &dp_register_set::operator &= (dp_register r) {
	bool ok = includes(r);
	reset();
	if (ok) *this += r;
	return *this;
}

dp_register_set &dp_register_set::operator &= (const dp_register_set &r) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {

		_data[ix] &= r._data[ix];
	}

	if (!r._extra) _extra = nullopt;

	if (_extra && r._extra) {
		std::vector<dp_register> tmp;
		std::set_intersection(
			_extra->begin(), _extra->end(), 
			r._extra->begin(), r._extra->end(),
			std::back_inserter(tmp)
		);
		_extra = std::move(tmp);
	}

	normalize_extra();
	return *this;
}

void dp_register_set::reset(void) {

	for (auto &bits : _data) bits.reset();
	_extra = nullopt;
}



dp_register_set::operator bool() const {
	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		if (_data[ix].any()) return true;
	}
	if (_extra && _extra->size()) return true;

	return false;
}



std::vector<dp_register> dp_register_set::registers(char type) const {

	int ix = reg_to_index(type);
	if (ix == -1) throw std::out_of_range("dp_register_set::registers");

	std::vector<dp_register> rv;
	if (_data[ix].none()) return rv;

	rv.reserve(_data[ix].count());

	for (unsigned i = 0; i < _data[ix].size(); ++i) {
		if (_data[ix].test(i)) {
			rv.push_back(dp_register{(unsigned)type, i});
		}
	}

	if (_extra) for (const auto &dp : *_extra) {
		if (dp.type == type) rv.push_back(dp);
	}

	return rv;
}

std::vector<dp_register> dp_register_set::registers() const {

	std::vector<dp_register> rv;

	for (unsigned ix = 0; ix < IndexCount; ++ix) {

		const auto &data = _data[ix];
		if (data.none()) continue;
		char type = index_to_reg(ix);

		for(unsigned i = 0; i < data.size(); ++i) {
			if (data.test(i)) {
				rv.push_back(dp_register{(unsigned)type, i});
			}
		}
	}

	if (_extra) for (const auto &dp : *_extra) {
		rv.push_back(dp);
	}

	return rv;
}

void dp_register_set::normalize_extra() {
	if (_extra && _extra->empty()) _extra = nullopt;
}

namespace { 
	void dump_one(char type, const std::bitset<64> &data, const optional<std::vector<dp_register>> &extra) {
		printf("%c:", type);
		for (unsigned i = 0; i < data.size(); ++i) {
			if (data[i]) printf(" %u", i);
		}
		if (extra) for (const auto &dp : *extra) {
			if (dp.type == type) printf(" %u", dp.number);
		}
		printf("\n");
	}
}
void dp_register_set::dump() const {
	dump_one('t', _data[0], _extra);
	dump_one('r', _data[1], _extra);
	dump_one('v', _data[2], _extra);
	dump_one('p', _data[3], _extra);
}
