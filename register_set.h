#ifndef __register_set_h__
#define __register_set_h__

#include <stdint.h>

/*
 * used to track which registers an opcode reads and writes
 * (1 set for reading, one set for writing)
 *
 */
struct register_set {

public:

	constexpr register_set(uint32_t data = 0) noexcept : bits(data & mask) {}
	constexpr register_set(const register_set &) noexcept = default;

	constexpr register_set& operator=(const register_set &) noexcept = default;
	constexpr register_set& operator=(uint32_t data) noexcept
	{
		bits = data & mask;
		return *this;
	}

	constexpr bool operator==(const register_set &rhs) const noexcept { return rhs.bits == bits; }
	constexpr bool operator==(uint32_t data) const noexcept { return (data & mask) == bits; }

	constexpr bool operator!=(const register_set &rhs) const noexcept { return rhs.bits != bits; }
	constexpr bool operator!=(uint32_t data) const noexcept { return (data & mask) != bits; }


	constexpr explicit operator bool() const noexcept {
		return bits;
	}

	constexpr bool operator!() const noexcept {
		return !bits;
	}


	constexpr register_set& operator|=(const register_set &rhs) noexcept {
		bits |= rhs.bits;
		return *this;
	}

	constexpr register_set& operator&=(const register_set &rhs) noexcept {
		bits &= rhs.bits;
		return *this;
	}

	constexpr register_set& operator^=(const register_set &rhs) noexcept {
		bits ^= rhs.bits;
		return *this;
	}


	constexpr register_set& operator+=(const register_set &rhs) noexcept {
		bits |= rhs.bits;
		return *this;
	}

	constexpr register_set& operator-=(const register_set &rhs) noexcept {
		bits &= (~rhs.bits) & mask;
		return *this;
	}

	constexpr register_set operator~(void) const noexcept {
		return register_set(~bits);
	}

	constexpr register_set operator|(const register_set &rhs) const noexcept {
		return register_set(bits | rhs.bits);
	}

	constexpr register_set operator&(const register_set &rhs) const noexcept {
		return register_set(bits & rhs.bits);
	}

	constexpr register_set operator^(const register_set &rhs) const noexcept {
		return register_set(bits ^ rhs.bits);
	}

	constexpr register_set operator+(const register_set &rhs) const noexcept {
		return register_set(bits | rhs.bits);
	}

	constexpr register_set operator-(const register_set &rhs) const noexcept {
		return register_set(bits & ~rhs.bits);
	}


	/*
	 * returns true if this is a superset of rhs.
	 */ 
	constexpr bool includes(const register_set &rhs) const noexcept {
		return (bits | rhs.bits) == bits;
	}

	constexpr bool a(void) const noexcept { return bits & a_mask; }
	constexpr bool x(void) const noexcept { return bits & x_mask; }
	constexpr bool y(void) const noexcept { return bits & y_mask; }
	constexpr bool d(void) const noexcept { return bits & d_mask; }
	constexpr bool s(void) const noexcept { return bits & s_mask; }
	constexpr bool pc(void) const noexcept { return bits & pc_mask; }
	constexpr bool b(void) const noexcept { return bits & b_mask; }
	constexpr bool k(void) const noexcept { return bits & k_mask; }

	constexpr uint16_t p(void) const noexcept { return bits & p_mask; }

	constexpr bool memory(void) const noexcept { return bits & mem_mask; }
	constexpr bool stack(void) const noexcept { return bits & stack_mask; }
	constexpr bool zp(void) const noexcept { return bits & zp_mask; }



	constexpr static register_set register_mask() {
		return register_set(rmask);
	}

	constexpr static register_set memory_mask() {
		return register_set(mmask);
	}

	void dump() const;



private:


	enum {
		a_mask = 1 << 0,
		x_mask = 1 << 1,
		y_mask = 1 << 2,
		d_mask = 1 << 3,
		s_mask = 1 << 4,
		b_mask = 1 << 5,
		k_mask = 1 << 6,
		pc_mask = 1 << 7,
		p_mask = 0x1ff << 8,

		// read/write memory.
		mem_mask = 1 << 24,
		stack_mask = 1 << 25,
		zp_mask = 1 << 26,

		rmask = 0x0001ffff,
		mmask = 0x05000000,
		mask = rmask+mmask,
	};


	uint32_t bits;

};


#endif