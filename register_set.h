#ifndef __register_set_h__
#define __register_set_h__

#include <array>
#include <bitset>
#include <vector>
#include <functional>

#include "dp_register.h"


#define RS_BITSET


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
	bool contains(dp_register r, unsigned count);
	bool contains(const register_set &);

	bool contains_any(dp_register r, unsigned count = 1);

	void insert(dp_register r, unsigned count = 1);
	void remove(dp_register r, unsigned count = 1);

	register_set &operator += (dp_register r);
	register_set &operator += (const register_set &);

	register_set &operator -= (dp_register r);
	register_set &operator -= (const register_set &);

	register_set &operator &= (dp_register r);
	register_set &operator &= (const register_set &);

	friend bool operator == (const register_set &, const register_set &);
	friend bool operator != (const register_set &, const register_set &);
	friend bool operator < (const register_set &, const register_set &);

	operator bool() const;

	#ifdef RS_BITSET
	std::bitset<32> bits(char type) const;
	#endif

	std::vector<dp_register> registers(char type) const;

	void dump() const;

private:

	#ifdef RS_BITSET
	// lcc is currently limited to 32 registers, so this is good enough for now.
	// hmm .. no longer true :( -- use optional<vector<dp_register>> for overflow!
	std::array< std::bitset<32>, 4> _data;
	#else
	std::array< std::vector<bool>, 4> _data;
	#endif
};

#endif

