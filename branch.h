#ifndef __branch_h__
#define __branch_h__

#include <cstdint>
#include <string>
#include <vector>
#include "Expression.h"

struct BasicLine;
struct branch {

	enum {
		//never,
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

	} type; // = always;
	bool far; // = false;


	enum {
		c = 1,
		n = 2,
		z = 4,
		v = 8
	};

	unsigned size() const;
	bool reads_c() const;
	bool reads_v() const;
	bool is_signed() const;
	bool is_conditional() const {
		return type != always;
	}

	bool reads_a() const;
	bool writes_a() const;

	unsigned flags() const;


	bool in_range(uint32_t pc, uint32_t target) const;

	unsigned make_far(); // sets far, returns # of additional bytes.

	std::string to_string() const;
	std::vector<BasicLine *> to_code(ExpressionPtr target) const;
};


#endif
