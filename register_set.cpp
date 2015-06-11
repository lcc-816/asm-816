

#include "register_set.h"

#include <stdexcept>

namespace {

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
}


#ifdef RS_BITSET

bool register_set::contains(dp_register r) {
	int ix = reg_to_index(r);
	if (ix < 0) return false;
	return _data[ix].size() > r.number && _data[ix][r.number]; 
}


bool register_set::contains(const register_set &rhs) {
	for (unsigned ix = 0; ix < 3; ++ix) {
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

	if (_data[ix].size() < r.number) return *this;

	_data[ix][r.number] = true;
	return *this;
}

register_set &register_set::operator += (const register_set &r) {

	for (unsigned ix = 0; ix < 3; ++ix) {

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

	for (unsigned ix = 0; ix < 3; ++ix) {
		auto tmp = r._data[ix];
		tmp.flip();
		_data[ix] &= tmp;
	}

	return *this;
}

register_set::operator bool() const {
	for (unsigned ix = 0; ix < 3; ++ix) {
		if (_data[ix].any()) return true;
	}
	return false;
}

std::bitset<32> register_set::bits(char type) const {
	int index = reg_to_index(type);
	if (index == -1) throw std::out_of_range("register_set::bits");

	return _data[index];
}

std::vector<dp_register> register_set::registers(char type) const {

	int ix = reg_to_index(type);
	if (ix == -1) throw std::out_of_range("register_set::registers");

	std::vector<dp_register> rv;
	rv.reserve(_data[ix].count());

	for (unsigned i = 0; i < _data[ix].size(); ++i) {
		if (_data[ix].test(i)) {
			rv.push_back(dp_register{(unsigned)type, i});
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
	for (unsigned ix = 0; ix < 3; ++ix) {
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

	for (unsigned ix = 0; ix < 3; ++ix) {
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

	for (unsigned ix = 0; ix < 3; ++ix) {
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

