
#include <array>
#include <bitset>
#include <vector>

#include "common.h"

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


private:
	#define RS_BITSET

	#ifdef RS_BITSET
	// lcc is currently limited to 32 registers, so this is good enough for now.
	std::array< std::bitset<32>, 3> _data;
	#else
	std::array< std::vector<bool>, 3> _data;
	#endif
};