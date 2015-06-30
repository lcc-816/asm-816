#ifndef __dp_register_h__
#define __dp_register_h__

#include <functional> // std::hash


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

	bool operator == (const dp_register &rhs) const {
		return (type == rhs.type) && (number == rhs.number);
	}
	bool operator != (const dp_register &rhs) const {
		return (type != rhs.type) || (number != rhs.number);
	}

	bool operator < (const dp_register &rhs) const {
		if (type < rhs.type) return true;
		if (number < rhs.number) return true;
		return false;
	}
};

#if 0
// having them out here breaks peephole reg_a != reg_d for some reason.
inline bool operator==(const dp_register &a, const dp_register &b) {
	return (a.type == b.type) && (a.number == b.number);
}

inline bool operator!=(const dp_register &a, const dp_register &b) {
	return (a.type != b.type) || (a.number != b.number);
}

/* for std::sort */
inline bool operator<(const dp_register &a, const dp_register &b) {
	if (a.type < b.type) return true;
	if (a.number < b.number) return true;
	return false;
}
#endif

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


#endif