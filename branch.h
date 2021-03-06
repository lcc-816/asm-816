#ifndef __branch_h__
#define __branch_h__

#include <cstdint>
#include <string>
#include <vector>

#include "types.h"
#include "register_set.h"

struct branch {

	enum branch_type {
		never,
		always,
		eq,
		ne,
		cc,
		cs,
		vs,
		vc,
		mi,
		pl,
		unsigned_gt,
		unsigned_ge,
		unsigned_lt,
		unsigned_le,

		signed_gt,
		signed_ge,
		signed_lt,
		signed_le,

	} type = always;
	bool far = false;


	enum {
		c = 1 << 0,
		z = 1 << 1,
		v = 1 << 6,
		n = 1 << 7,
	};

	unsigned size() const;
	bool reads_c() const;
	bool reads_v() const;
	bool is_signed() const;
	bool is_conditional() const {
		return type != always && type != never;
	}

	bool reads_a() const;
	bool writes_a() const;

	unsigned flags() const;


	bool operator==(const branch &rhs) const {
		return type == rhs.type && far == rhs.far;
	}

	bool operator!=(const branch &rhs) const {
		return type != rhs.type || far != rhs.far;
	}

	bool in_range(uint32_t pc, uint32_t target) const;

	unsigned make_far(); // sets far, returns # of additional bytes.

	static branch_type invert(branch_type type);
	branch operator!() const;

	branch make_signed() const;
	static branch::branch_type make_signed(branch_type type);

	std::string to_string() const;
	std::vector<BasicLinePtr> to_code(ExpressionPtr target) const;

	register_set read_registers() const;
	register_set write_registers() const;
};


#endif
