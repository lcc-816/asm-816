

#include "register_set.h"

#include <stdexcept>

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

template<class T, class BinaryFunction>
void with(T &lhs, const T &rhs, BinaryFunction b) {
	static_assert(lhs.size() == rhs.size(), "oops");
	for (unsigned i = 0; i < lhs.size(); ++i)
		b(lhs[i], rhs[i]);
}

#ifdef RS_BITSET

bool register_set::contains(dp_register r) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;
	return _data[ix].size() > r.number && _data[ix][r.number]; 
}

bool register_set::contains(dp_register r, unsigned count) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;
	for (unsigned i = 0; i < count; ++i) {
		if (_data[ix].size() <= r.number) return false;
		if (!_data[ix][r.number]) return false;
		r.number += 2;
	}
	return true;
}

bool register_set::contains_any(dp_register r, unsigned count) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;
	for (unsigned i = 0; i < count; ++i) {
		if (_data[ix].size() > r.number && _data[ix][r.number]) return true;
	}
	return false;
}


bool register_set::contains(const register_set &rhs) {
	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		auto tmp = _data[ix];
		tmp.flip();
		tmp &= rhs._data[ix];
		if (tmp.count()) return false;
	}
	return true;
}

register_set &register_set::operator += (dp_register r) {
	int ix = reg_to_index(r);
	if (ix < 0) return *this;


	if (_data[ix].size() < r.number) {
		throw std::runtime_error("too many registers (bitset exceeded)");
		return *this;
	}
	_data[ix][r.number] = true;
	return *this;
}


void register_set::insert(dp_register r, unsigned count) {
	unsigned i;
	for (i = 0; i < count; ++i) {
		*this += r;
		r.number += 2;
	}
}

void register_set::remove(dp_register r, unsigned count) {
	unsigned i;
	for (i = 0; i < count; ++i) {
		*this -= r;
		r.number += 2;
	}
}



register_set &register_set::operator += (const register_set &r) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {

		_data[ix] |= r._data[ix];
	}

	return *this;
}

register_set &register_set::operator -= (dp_register r) {

	int ix = reg_to_index(r);
	if (ix < 0) return *this;

	if (_data[ix].size() > r.number)
		_data[ix][r.number] = false;

	return *this;	
}


register_set &register_set::operator -= (const register_set &r) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		auto tmp = r._data[ix];
		tmp.flip();
		_data[ix] &= tmp;
	}

	return *this;
}

register_set &register_set::operator &= (dp_register r) {
	bool ok = contains(r);
	reset();
	if (ok) *this += r;
	return *this;
}

register_set &register_set::operator &= (const register_set &r) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {

		_data[ix] &= r._data[ix];
	}

	return *this;
}

void register_set::reset(void) {

	for (auto &bits : _data) bits.reset();

}



register_set::operator bool() const {
	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		if (_data[ix].any()) return true;
	}
	return false;
}

#if 0
std::bitset<32> register_set::bits(char type) const {
	int index = reg_to_index(type);
	if (index == -1) throw std::out_of_range("register_set::bits");

	return _data[index];
}
#endif

std::vector<dp_register> register_set::registers(char type) const {

	int ix = reg_to_index(type);
	if (ix == -1) throw std::out_of_range("register_set::registers");

	std::vector<dp_register> rv;
	if (_data[ix].none()) return rv;

	rv.reserve(_data[ix].count());

	for (unsigned i = 0; i < _data[ix].size(); ++i) {
		if (_data[ix].test(i)) {
			rv.push_back(dp_register{(unsigned)type, i});
		}
	}
	return rv;
}

std::vector<dp_register> register_set::registers() const {

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
	
	return rv;
}


#else


bool register_set::contains(dp_register r) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;
	return _data[ix].size() > r.number && _data[ix][r.number]; 
}


bool register_set::contains(const dp_register &rhs) {
	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		int s1 = _data[ix].size();
		int s2 = r._data[ix].size();
		int l = std::min(s1,s2);
		for (int i = 0; i < l; ++i) {
			if (rhs._data[ix][i] && !_data[ix][i]) return false;
		}

		// any bits beyond l => NO.
		for (int i = l; i < s2; ++i) {
			if (rhs._data[ix][i]) return false;
		}
	}

	return true;
}


register_set &register_set::operator += (dp_register r) {
	int ix = reg_to_index(r);
	if (ix < 0) return *this;

	if (_data[ix].size() <= r.number) {
		_data[ix].resize(r.number + 1, false);
	}
	_data[ix][r.number] = true;
	return *this;
}

register_set &register_set::operator += (const register_set &r) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		int size = r._data[ix].size();
		if (_data[ix].size() < size)
			_data[ix].resize(size, false);
		for (unsigned i = 0; i < size; ++i)
			if (r._data[ix][i]) _data[ix][i] = true;
	}

	return this;
}

register_set &register_set::operator -= (dp_register r) {

	int ix = reg_to_index(r);
	if (ix < 0) return *this;

	if (_data[ix].size() > r.number)
		_data[ix][r.number] = false;

	return *this;	
}

register_set &register_set::operator -= (const register_set &r) {

	for (unsigned ix = 0; ix < IndexCount; ++ix) {
		int s1 = _data[ix].size();
		int s2 = r._data[ix].size();
		int l = std::min(s1,s2);
		for (int i = 0; i < l; ++i) {
			if (rhs._data[ix][i]) _data[ix][i] = false;
		}		
	}

	return *this;
}

#endif

namespace { 
	void dump_one(char type, const std::bitset<32> &data) {
		printf("%c:", type);
		for (unsigned i = 0; i < data.size(); ++i) {
			if (data[i]) printf(" %u", i);
		}
		printf("\n");
	}
}
void register_set::dump() const {
	dump_one('t', _data[0]);
	dump_one('r', _data[1]);
	dump_one('v', _data[2]);
	dump_one('p', _data[3]);
}
