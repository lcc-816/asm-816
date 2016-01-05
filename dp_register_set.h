#ifndef __dp_register_set_h__
#define __dp_register_set_h__

#include <array>
#include <bitset>
#include <vector>
#include <functional>

#include "dp_register.h"


#define RS_BITSET

class dp_register_pair {
	struct dp_register r;
	unsigned size = 0;
};


class dp_register_set
{
public:


	dp_register_set() = default;
	dp_register_set(dp_register_set &&) = default;
	dp_register_set(const dp_register_set &) = default;
	~dp_register_set() = default;

	dp_register_set &operator=(const dp_register_set &) = default;
	dp_register_set &operator=(dp_register_set &&) = default;

	bool contains(dp_register r);
	bool contains(dp_register r, unsigned count);
	bool contains(const dp_register_set &);

	bool contains_any(dp_register r, unsigned count = 1);

	void insert(dp_register r, unsigned count = 1);
	void remove(dp_register r, unsigned count = 1);

	dp_register_set &operator += (dp_register r);
	dp_register_set &operator += (const dp_register_set &);

	dp_register_set &operator -= (dp_register r);
	dp_register_set &operator -= (const dp_register_set &);

	dp_register_set &operator &= (dp_register r);
	dp_register_set &operator &= (const dp_register_set &);

	friend bool operator == (const dp_register_set &, const dp_register_set &);
	friend bool operator != (const dp_register_set &, const dp_register_set &);
	friend bool operator < (const dp_register_set &, const dp_register_set &);

	explicit operator bool() const;

	#if 0
	#ifdef RS_BITSET
	std::bitset<32> bits(char type) const;
	#endif
	#endif

	std::vector<dp_register> registers(char type) const;
	std::vector<dp_register> registers() const;

	void dump() const;

private:

	void reset();
	
	#ifdef RS_BITSET
	// lcc is currently limited to 32 registers, so this is good enough for now.
	// hmm .. no longer true :( -- use optional<vector<dp_register>> for overflow!
	std::array< std::bitset<32>, 4> _data;
	#else
	std::array< std::vector<bool>, 4> _data;
	#endif
};

#endif

