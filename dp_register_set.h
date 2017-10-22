#ifndef __dp_register_set_h__
#define __dp_register_set_h__

#include <array>
#include <bitset>
#include <vector>
#include <functional>
#include <initializer_list>

#include "dp_register.h"


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

	dp_register_set(std::initializer_list<dp_register> l);
	dp_register_set(const std::vector<dp_register> &v);

	~dp_register_set() = default;

	dp_register_set &operator=(const dp_register_set &) = default;
	dp_register_set &operator=(dp_register_set &&) = default;

	bool includes(dp_register r);
	bool includes(dp_register r, unsigned count);
	bool includes(const dp_register_set &);

	bool includes_any(dp_register r, unsigned count = 1);

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


	std::vector<dp_register> registers(char type) const;
	std::vector<dp_register> registers() const;

	void dump() const;

private:

	void reset();
	
	// any overflow goes into the _extra vector which is sorted.
	std::array< std::bitset<64>, 4> _data;
	std::vector<dp_register> _extra;
};

#endif

