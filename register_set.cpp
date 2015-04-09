

#include "register_set.h"

namespace {
	inline int reg_to_index(dp_register r) {
		switch (r.type) {
			case 't': return 0;
			case 'v': return 1;
			case 'p': return 2;
			default:
				return -1;
		}
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

