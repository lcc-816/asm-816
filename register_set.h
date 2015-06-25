#ifndef __register_set_h__
#define __register_set_h__

#include <array>
#include <bitset>
#include <vector>
#include <functional>

/*
 * used by lemon grammar, so no constructors allowed.
 */

struct dp_register {
	unsigned type;
	unsigned number;

	static dp_register make(unsigned type = 0, unsigned number = 0) {
		return dp_register({type, number});
	}

	bool is_temporary() const { return type == 'r' || type == 't'; }
	operator bool() const { return (bool)type; }
	dp_register &operator += (int i) { number += i; return *this;}
};


inline bool operator==(const dp_register &a, const dp_register &b) {
	return a.type == b.type && a.number == b.number;
}

/* for std::sort */
inline bool operator<(const dp_register &a, const dp_register &b) {
	if (a.type < b.type) return true;
	if (a.number < b.number) return true;
	return false;
}

inline dp_register operator+(dp_register r, int i) {
	r.number += i;
	return r;
}

namespace std {
	template<>
	struct hash<dp_register> {

		typedef dp_register argument_type;
		typedef std::size_t result_type;

		result_type operator()(const argument_type &r) const {
			return static_cast<result_type>(r.type ^ (r.number << 8));
		}
	};
}

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

	friend bool operator == (const register_set &, const register_set &);
	friend bool operator != (const register_set &, const register_set &);
	friend bool operator < (const register_set &, const register_set &);

	operator bool() const;

	#ifdef RS_BITSET
	std::bitset<32> bits(char type) const;
	#endif

	std::vector<dp_register> registers(char type) const;

private:

	#ifdef RS_BITSET
	// lcc is currently limited to 32 registers, so this is good enough for now.
	// hmm .. no longer true :(
	std::array< std::bitset<32>, 4> _data;
	#else
	std::array< std::vector<bool>, 4> _data;
	#endif
};

#endif

