/*
 *
 * pseudo branches.
 *
 */

#include "branch.h"
#include "types.h"
#include "Expression.h"
#include "common.h"

bool branch::is_signed() const {

	switch(type) {
		case signed_gt:
		case signed_ge:
		case signed_lt:
		case signed_le:
			return true;
		default:
			return false;
	}

}

bool branch::reads_c() const {
	switch (type) {
		case unsigned_gt:
		case unsigned_ge:
		case unsigned_lt:
		case unsigned_le:
		case cc:
		case cs:
			return true;
		default:
			return false;
	}
}

bool branch::reads_v() const {
	switch(type) {
		case signed_gt:
		case signed_ge:
		case signed_lt:
		case signed_le:
		case vs:
		case vc:
			return true;
		default:
			return false;
	}
}

bool branch::reads_a() const {
	switch(type) {
		case signed_gt:
		case signed_ge:
		case signed_lt:
		case signed_le:
			return true;
		default:
			return false;
	}
}
bool branch::writes_a() const {
	switch(type) {
		case signed_gt:
		case signed_ge:
		case signed_lt:
		case signed_le:
			return true;
		default:
			return false;
	}
}


unsigned branch::size() const {
	
	switch(type) {
	case always:
		// bra xxx
		// brl xxx
		return far ? 3 : 2;
	case eq:
	case ne:
	case cc:
	case cs:
	case vs:
	case vc:
	case mi:
	case pl:
		// beq xxx
		// bne *+5 brl xxx
		return far ? 5 : 2;

	case unsigned_ge:
		return far ? 5 : 2;
	case unsigned_lt:
		return far ? 5 : 2;

	case unsigned_gt:
		// beq *+4 bcs xxx 
		// beq *+7 bcc *+5 brl xxxx
		return far ? 7 : 4;

	case unsigned_le:
		// beq xxx bcc xxx  
		// beq *+4 bcs *+5 brl xxx
		return far ? 7 : 4;

		// for signed ops, lcc has previously generated sec, sbc
		// instead of cmp #...
	case signed_ge:
		// N == V
		// bvs *+5 eor #$8000 bmi xxx
		// bvs *+5 eor #$8000 bpl *+5 brl xxx
		return far ? 10 : 7;

	case signed_lt:
		// N != V
		// bvs *+5 eor #$8000 bpl xxx
		// bvs *+5 eor #$8000 bmi *+5 brl xxx
		return far ? 10 : 7;

	case signed_gt:
		// N == V, !Z
		// beq *+9 bvs *+5 eor #$8000 bmi xxx
		// beq *+12 bvs *+5 eor #$8000 bpl *+5 brl xxx
		return far ? 12 : 9;

	case signed_le:
		// N != V || Z
		// beq xxx bvs *+5 eor #$8000 bpl xxx
		// beq *+9 bvs *+5 eor #$8000 bmi *+5 brl xxx
		return far ? 12 : 9;
	}
	return 0;
}

bool branch::in_range(uint32_t pc, uint32_t target) const {

	int32_t offset;

	if (far) return true; // 64-k bank check elsewhere.

	unsigned pc_adj = 2;
	switch (type) {
		case unsigned_gt:
			pc_adj = 4;
			break;
		case signed_gt:
			pc_adj = 7;
			break;
		case signed_lt:
			pc_adj = 7;
			break;
		case signed_ge:
			pc_adj = 7;
			break;

		default:
			break;
	}

	// simple types.
	switch (type) {
		case always:
		case eq:
		case ne:
		case cc:
		case cs:
		case vs:
		case vc:
		case mi:
		case pl:
		case unsigned_gt:
		case unsigned_ge:
		case unsigned_lt:
		case signed_gt:
		case signed_ge:
		case signed_lt:
			{
				offset = target - (pc+pc_adj);
				if (offset > 127 || offset < -128) return false;
				return true;
			}
		// <= has two branches to consider.
		case unsigned_le:
			{
				pc_adj = 2;
				offset = target - (pc+pc_adj);
				if (offset > 127 || offset < -128) return false;

				pc_adj = 4;
				offset = target - (pc+pc_adj);
				if (offset > 127 || offset < -128) return false;

				return true;
			}
		case signed_le:
			{
				pc_adj = 2;
				offset = target - (pc+pc_adj);
				if (offset > 127 || offset < -128) return false;

				pc_adj = 9;
				offset = target - (pc+pc_adj);
				if (offset > 127 || offset < -128) return false;

				return true;
			}
	}

}


unsigned branch::make_far() {
	if (far) return 0;
	far = true;
	switch(type) {
		case always:
			return 1;
		default:
			return 3;
	}

}

std::string branch::to_string() const {

	static std::string names[] = {
		"__bra",
		"__beq",
		"__bne",
		"__bcc",
		"__bcs",
		"__bvs",
		"__bvc",
		"__bmi",
		"__bpl",


		"__bugt",
		"__buge",
		"__bult",
		"__bule",

		"__bsgt",
		"__bsge",
		"__bslt",
		"__bsle",
	};

	return names[type];
}

namespace {
	ExpressionPtr star_plus(unsigned offset) {
		// generate an expression ptr for *+offset
		return Expression::Binary('+', Expression::PC(), Expression::Integer(offset));
	}
}
std::vector<BasicLinePtr> branch::to_code(ExpressionPtr target) const {

	std::vector<BasicLinePtr> rv;

	switch(type) {
		case always:
			if (far) {
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BRA, relative, target));
			}
			break;

		case eq:
			if (far) {
				rv.emplace_back(BasicLine::Make(BNE, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BEQ, relative, target));
			}
			break;

		case ne:
			if (far) {
				rv.emplace_back(BasicLine::Make(BEQ, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BNE, relative, target));
			}
			break;

		case cc:
			if (far) {
				rv.emplace_back(BasicLine::Make(BCS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BCC, relative, target));
			}
			break;

		case cs:
			if (far) {
				rv.emplace_back(BasicLine::Make(BCC, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BCS, relative, target));
			}
			break;


		case vc:
			if (far) {
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BVC, relative, target));
			}
			break;

		case vs:
			if (far) {
				rv.emplace_back(BasicLine::Make(BVC, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BVS, relative, target));
			}
			break;


		case mi:
			if (far) {
				rv.emplace_back(BasicLine::Make(BPL, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BMI, relative, target));
			}
			break;

		case pl:
			if (far) {
				rv.emplace_back(BasicLine::Make(BMI, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BPL, relative, target));
			}
			break;


		case unsigned_ge:
			if (far) {
				rv.emplace_back(BasicLine::Make(BCC, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BCS, relative, target));
			}
			break;

		case unsigned_lt:
			if (far) {
				rv.emplace_back(BasicLine::Make(BCS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BCC, relative, target));
			}
			break;

		case unsigned_gt:
			if (far) {
				rv.emplace_back(BasicLine::Make(BEQ, relative, star_plus(7)));
				rv.emplace_back(BasicLine::Make(BCC, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				rv.emplace_back(BasicLine::Make(BEQ, relative, star_plus(4)));
				rv.emplace_back(BasicLine::Make(BCS, relative, target));
			}
			break;


		case unsigned_le:
			if (far) {
				rv.emplace_back(BasicLine::Make(BEQ, relative, star_plus(4)));
				rv.emplace_back(BasicLine::Make(BCS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));
			} else {
				// assume bcc more likely than beq
				rv.emplace_back(BasicLine::Make(BCC, relative, target));
				rv.emplace_back(BasicLine::Make(BEQ, relative, target));
			}
			break;


		case signed_ge:
			if (far) {
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BPL, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));

			} else {
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BMI, relative, target));
			}
			break;

		case signed_lt:
			if (far) {
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BMI, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));

			} else {
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BPL, relative, target));
			}
			break;

		case signed_gt:
			if (far) {
				rv.emplace_back(BasicLine::Make(BEQ, relative, star_plus(12)));
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BPL, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));

			} else {
				rv.emplace_back(BasicLine::Make(BEQ, relative, star_plus(9)));
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BMI, relative, target));
			}
			break;


		case signed_le:
			if (far) {
				rv.emplace_back(BasicLine::Make(BEQ, relative, star_plus(9)));
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BMI, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(BRL, relative_long, target));

			} else {
				rv.emplace_back(BasicLine::Make(BEQ, relative, target));
				rv.emplace_back(BasicLine::Make(BVS, relative, star_plus(5)));
				rv.emplace_back(BasicLine::Make(EOR, immediate, Expression::Integer(0x8000)));
				rv.emplace_back(BasicLine::Make(BPL, relative, target));
			}
			break;

	}

	return rv;
}


unsigned branch::flags() const {
	switch(type) {
		case always: return 0;
		case eq:
		case ne:
			return z;
		case cc:
		case cs:
		case unsigned_ge:
		case unsigned_lt:
			return c;
		case vs:
		case vc:
			return v;
		case pl:
		case mi:
			return n;
		case unsigned_gt:
		case unsigned_le:
			return c | z;
		case signed_lt:
		case signed_ge:
			return n | v;
		case signed_le:
		case signed_gt:
			return n | v | z;
	}
}

branch::branch_type branch::invert(branch_type type) {
	switch(type) {
		default:
		case always:
			// oops
			return type;
		case eq:
			return ne;
		case ne:
			return eq;
		case cc:
			return cs;
		case cs:
			return cc;
		case vs:
			return vc;
		case vc:
			return vs;
		case mi:
			return pl;
		case pl:
			return mi;
		case unsigned_gt:
			return unsigned_le;
		case unsigned_ge:
			return unsigned_lt;
		case unsigned_lt:
			return unsigned_ge;
		case unsigned_le:
			return unsigned_gt;

		case signed_gt:
			return signed_le;
		case signed_ge:
			return signed_lt;
		case signed_lt:
			return signed_ge;
		case signed_le:		
			return signed_gt;
	}
}

branch branch::operator!() const {
	return branch{ invert(type), far };
}


register_set branch::read_registers() const {
	switch(type) {
		case always:
			return register_set(0);
		case eq:
		case ne:
			return register_set(z << 8);
		case cc:
		case cs:
		case unsigned_ge:
		case unsigned_lt:
			return register_set(c << 8);
		case vc:
		case vs:
			return register_set(v << 8);
		case mi:
		case pl:
			return register_set(n << 8);

		case unsigned_gt:
		case unsigned_le:
			return register_set((z | c) << 8);

		case signed_ge:
		case signed_lt:
			return register_set((v << 8) + 1);
		case signed_le:
		case signed_gt:
			return register_set(((z|v) << 8) + 1);
	}
}

register_set branch::write_registers() const {
	switch(type) {
		case always:
		case eq:
		case ne:
		case cc:
		case cs:
		case vc:
		case vs:
		case mi:
		case pl:
		case unsigned_gt:
		case unsigned_ge:
		case unsigned_lt:
		case unsigned_le:
			return register_set(0x80);
		case signed_gt:
		case signed_ge:
		case signed_lt:
		case signed_le:
			// eor may set the n/z bits, but eor might not be executed.
			return register_set(((n|z) << 8) + 0x80);
	}
}
