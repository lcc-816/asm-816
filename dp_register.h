#ifndef __dp_register_h__
#define __dp_register_h__

// std::hash
#include <functional> 


struct dp_register {
	unsigned type = 0;
	unsigned number = 0;

	constexpr dp_register() noexcept = default;
	constexpr dp_register(unsigned t, unsigned n) noexcept : type(t), number(n)
	{}

	constexpr dp_register(const dp_register &) noexcept = default;


	constexpr bool is_temporary() const noexcept { return type == 'r' || type == 't'; }
	constexpr explicit operator bool() const noexcept { return (bool)type; }
	constexpr dp_register &operator += (int i) noexcept { number += i; return *this;}

	// currently +2.  should change to +1 at some point!
	constexpr dp_register &operator++() { number += 2; return *this; }

	constexpr bool operator == (const dp_register &rhs) const noexcept {
		return (type == rhs.type) && (number == rhs.number);
	}

	constexpr bool operator != (const dp_register &rhs) const noexcept {
		return (type != rhs.type) || (number != rhs.number);
	}

	constexpr bool operator < (const dp_register &rhs) const noexcept {
		if (type < rhs.type) return true;
		if (type > rhs.type) return false;
		return (number < rhs.number);
	}

	constexpr bool operator <= (const dp_register &rhs) const noexcept {
		if (type < rhs.type) return true;
		if (type > rhs.type) return false;
		return number <= rhs.number;
	}


	constexpr bool operator > (const dp_register &rhs) const noexcept {
		if (type > rhs.type) return true;
		if (type < rhs.type) return false;
		return number > rhs.number;
	}

	constexpr bool operator >= (const dp_register &rhs) const noexcept {
		if (type > rhs.type) return true;
		if (type < rhs.type) return false;
		return number >= rhs.number;
	}


};



inline constexpr dp_register operator+(dp_register r, int i) {
	r.number += i;
	return r;
}

inline constexpr dp_register operator+(int i, dp_register r) {
	r.number += i;
	return r;
}


namespace std {
	template<>
	struct hash<dp_register> {

		typedef dp_register argument_type;
		typedef std::size_t result_type;

		constexpr result_type operator()(const argument_type &r) const noexcept {
			return static_cast<result_type>(r.type ^ (r.number << 8));
		}
	};
}


#endif