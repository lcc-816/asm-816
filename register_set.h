#ifndef __register_set_h__
#define __register_set_h__

#include <array>
#include <bitset>
#include <vector>


struct dp_register {
	unsigned type;
	unsigned number;

	operator bool() const { return (bool)type; }
	dp_register &operator += (int i) { number += i; return *this;}
};

inline bool operator==(const dp_register &a, const dp_register &b) {
	return a.type == b.type && a.number == b.number;
}

inline dp_register operator+(dp_register r, int i) {
	r.number += i;
	return r;
}

class register_set
{
public:


	register_set() = default;
	register_set(register_set &&) = default;
	register_set(const register_set &) = default;
	~register_set() = default;

	register_set &operator=(const register_set &) = default;
	register_set &operator=(register_set &&) = default;

	bool contains(dp_register r);
	bool contains(const register_set &);

	register_set &operator += (dp_register r);
	register_set &operator += (const register_set &);

	register_set &operator -= (dp_register r);
	register_set &operator -= (const register_set &);

	friend bool operator == (const register_set &, const register_set &);
	friend bool operator != (const register_set &, const register_set &);
	friend bool operator < (const register_set &, const register_set &);

	operator bool() const;

private:
	#define RS_BITSET

	#ifdef RS_BITSET
	// lcc is currently limited to 32 registers, so this is good enough for now.
	std::array< std::bitset<32>, 3> _data;
	#else
	std::array< std::vector<bool>, 3> _data;
	#endif
};

#endif
