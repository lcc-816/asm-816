
#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"
#include "OpCode.h"
#include "register_set.h"
#include "optional_flags.h"

#include <experimental/optional>
#include <unordered_map>

using namespace std::experimental;


/*
 * this is like experimental::optional<uint16_t> but...
 * unknown values are tracked and comparable.
 *
 */
class reg_value {

public:

	reg_value(const reg_value &) = default;


	reg_value() : _engaged(false), _data(++_counter)
	{}

	reg_value(uint16_t value) : _engaged(true), _data(value)
	{}


	reg_value &operator=(const reg_value &) = default;
	reg_value &operator=(uint16_t value) {
		_engaged = true;
		_data = value & 0xffff;
		return *this;
	}

	reg_value &operator=(const std::experimental::nullopt_t) {
		invalidate();
		return *this;
	}

	operator bool() const {
		return _engaged;
	}

	bool operator !() const {
		return !_engaged;
	}

	uint16_t operator *() const {
		return _data;
	}

	uint16_t value() const {
		if (_engaged) return _data;
		throw std::experimental::bad_optional_access();
	}

	uint16_t value_or(uint16_t value) const {
		return _engaged ? _data : value;
	}

	void swap(reg_value &other) {
		std::swap(_engaged, other._engaged);
		std::swap(_data, other._data);
	}

	bool operator==(const reg_value &rhs) const {
		if (_engaged == rhs._engaged) return _data == rhs._data;
		return false;
	}

	bool operator==(uint16_t rhs) const {
		return _engaged && _data == rhs;
	}

	bool operator!=(const reg_value &rhs) const {
		if (_engaged == rhs._engaged) return _data != rhs._data;
		return true;
	}


	bool operator<(const reg_value &rhs) const {
		if (_engaged == rhs._engaged) return _data < rhs._data;

		// unknown < known.
		return !_engaged;
	}

	reg_value& operator += (uint16_t rhs) {
		//if (rhs == 0) return *this;

		modify([rhs](uint16_t value) { return value + rhs; });
		return *this;
	}

	reg_value& operator += (const reg_value &rhs) {
		if (rhs)
			modify([&rhs](uint16_t value) { return value + *rhs; });

		else invalidate();
		return *this;
	}



	reg_value& operator -= (uint16_t rhs) {
		//if (rhs == 0) return *this;

		modify([rhs](uint16_t value) { return value - rhs; });
		return *this;
	}

	reg_value& operator -= (const reg_value &rhs) {
		if (rhs)
			modify([&rhs](uint16_t value) { return value - *rhs; });

		else invalidate();
		return *this;
	}


	reg_value& operator |= (uint16_t rhs) {
		if (rhs == 0) return *this;

		modify([rhs](uint16_t value) { return value | rhs; });
		return *this;
	}

	reg_value& operator |= (const reg_value &rhs) {
		if (rhs)
			modify([&rhs](uint16_t value) { return value | *rhs; });

		else invalidate();
		return *this;
	}

	reg_value& operator &= (uint16_t rhs) {
		if (rhs == 0xffff) return *this;

		modify([rhs](uint16_t value) { return value & rhs; });
		return *this;
	}

	reg_value& operator &= (const reg_value &rhs) {
		if (rhs)
			modify([&rhs](uint16_t value) { return value & *rhs; });

		else invalidate();
		return *this;
	}


	reg_value& operator ^= (uint16_t rhs) {

		modify([rhs](uint16_t value) { return value ^ rhs; });
		return *this;
	}

	reg_value& operator ^= (const reg_value &rhs) {
		if (rhs)
			modify([&rhs](uint16_t value) { return value ^ *rhs; });

		else invalidate();
		return *this;
	}


	reg_value& operator <<= (uint16_t rhs) {

		modify([rhs](uint16_t value) { return value << rhs; });
		return *this;
	}

	reg_value& operator >>= (uint16_t rhs) {

		modify([rhs](uint16_t value) { return value >> rhs; });
		return *this;
	}


	void set_nz(optional_flags &f) {
		if (_engaged) {
			f.n() = _data & 0x8000;
			f.z() = !(_data & 0xffff);
		}
		else {
			f.n() = nullopt;
			f.z() = nullopt;
		}
	}


	template<class FX>
	void modify(FX fx) {
		if (_engaged) _data = fx(_data) & 0xffff;
		else _data = ++_counter;
	}


	static void reset_counter();

private:

	void invalidate() {
		_engaged = false;
		_data = ++_counter;
	}

	static unsigned _counter;
	bool _engaged;
	unsigned _data;
};

void reg_value::reset_counter() {
	_counter = 0;
}

unsigned reg_value::_counter = 0;

namespace std {
	void swap(reg_value &lhs, reg_value &rhs) {
		lhs.swap(rhs);
	}
}

/*
 *
 *
 */

optional<uint16_t> &operator += (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs + rhs);
	return lhs;
}

optional<uint16_t> &operator -= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs - rhs);
	return lhs;
}

optional<uint16_t> &operator |= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs | rhs);
	return lhs;
}

optional<uint16_t> &operator &= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs & rhs);
	return lhs;
}

optional<uint16_t> &operator ^= (optional<uint16_t> &lhs, uint16_t rhs) {
	if (lhs) lhs = (*lhs ^ rhs);
	return lhs;
}


static bool writes_memory(Mnemonic m) {

	switch(m) {
	case STA:
	case STX:
	case STZ:
	case INC:
	case DEC:
	case TRB:
	case TSB:
	case ASL:
	case LSR:
	case ROL:
	case ROR:
		return true;
	default:
		return false;
	}
}


template<class T, class FX>
void erase_if(T &t, FX fx) {

	for (auto it = t.begin(); it != t.end(); ) {
		if (fx(it->first, it->second)) {
			it = t.erase(it);
		} else {
			++it;
		}
	}
}

/*
 * for tracking values on the stack. invalidate after function calls or if instruction
 * alters S.
 */
typedef std::vector<reg_value> optional_stack;

reg_value pop(optional_stack &stack) {
	if (stack.empty()) return reg_value();

	reg_value v = stack.back();
	stack.pop_back();
	return v;
}

void push(optional_stack &stack, const reg_value &v) {
	stack.push_back(v);
}


void reg_inc(reg_value &r, optional_flags &flags) {
	r += 1;
	r.set_nz(flags);
}

void reg_dec(reg_value &r, optional_flags &flags) {
	r -= 1;
	r.set_nz(flags);
}





bool reg_const(LineQueue &list) {



	//typedef optional<uint16_t> register_type;
	typedef reg_value register_type;


	std::unordered_map<dp_register, register_type> dp_table;

	optional_stack stack;
	optional_flags flags;

	register_type reg_a;
	register_type reg_x;
	register_type reg_y;


	bool delta = false;

	LineQueue out;

	while (!list.empty()) {
		bool kill = false;

		BasicLinePtr line = list.front();
		list.pop_front();


		ExpressionPtr e = line->operands[0];
		const OpCode &op = line->opcode;

		/*
		 * todo --- also track the stack?
		 * std::stack<reg_type> stack
		 * jsl / jsr / tcs / txs / php / phb / phk etc clobber it.
		 * pla --> if stack empty, set to unknown value.  otherwise, pop value.
		 * track where it was pushed and kill push if redundant.
		 */

		if (op) {

			AddressMode mode = op.addressMode();
			Mnemonic mnemonic = op.mnemonic();

			register_set rs = line->read_registers();
			register_set ws = line->write_registers();

			// clobbers all %p and %v:
			// sta []
			// sta [],y
			// sta abslong,
			// sta abslong,x 
			// jsl
			// jsr

			// clobbers some %p or %v, based on #
			// sta zp,x -- generated by struct code?
			// stz zp,x

			optional_flags old_flags = flags;

			flags.reset(ws.p());

#if NOT_YET
			switch(mnemonic) {
					// todo -- long x/y
				case PHA:
					stack.push_back(reg_a);
					break;
				case PHX:
					stack.push_back(reg_x);
					break;
				case PHY:
					stack.push_back(reg_y);
					break;
				case PLA:
					reg_a = pop(stack);
					break;
				case PLX:
					reg_x = pop(stack);
					break;
				case PLY:
					reg_y = pop(stack);
					break;
				case PEA:
					{
						uint32_t v;
						if (e->operands[0] && e->operands[0]->is_integer(v)) {
							stack.push_back(reg_value(e));
						}
						else stack.clear();
					}
					break;
				case PEI:

				default:
					if (ws.s() || ws.stack())
						stack.clear();
			}
#endif



			if (mnemonic == JSL || mnemonic == JSR || mnemonic == RTS || mnemonic == RTL) {
				// invalidate all %p and %v variables
				erase_if(dp_table, [](const dp_register &k, const reg_value &v){
					return !k.is_temporary();
				});
				stack.clear();
			}

			if (writes_memory(mnemonic)) {
				switch (mode) {
					case zp_indirect:
					case zp_indirect_long:
					case absolute_long:
					case absolute_long_x:
						// invalidate all %p and %v variables
						for (auto it = dp_table.begin(); it != dp_table.end(); ) {
							if (it->first.is_temporary()) ++it;
							else it = dp_table.erase(it);
						}
						break;

					case zp_x:
					case zp_y:
						{
							// invalidate variables of the same class and >= value.
							dp_register reg;
							if (e->is_register(reg)) {
								// these should never be %r or %t vars, but either way...
								for (auto it = dp_table.begin(); it != dp_table.end(); ) {
									if (it->first.type == reg.type && it->first.number >= reg.number) 
										it = dp_table.erase(it);
									else ++it;
	
								}
							}
						}
						break;

					default:
						break;
				}
			}




			/*
			register_type old_a = reg_a;
			register_type old_x = reg_x;
			register_type old_y = reg_y;
			*/

			if (mode == zp || mnemonic == PEI) {
				dp_register reg;

				// doesn't matter if temporary... after sta [] / sta [],y, jsl, or jsr, clobber
				// all %v and %p variables.

				if (e->is_register(reg)) {

					register_type &dp_reg = dp_table[reg];

					register_type old_dp(dp_reg);

					// todo -- worry about %t1, etc.
					// todo -- worry about short m, etc.

					switch(mnemonic) {

						case ADC:
							if (reg_a && dp_reg && old_flags.c() && !flags.d().value_or(1)) {
								uint32_t tmp = *reg_a + *dp_reg + *flags.c();

								flags.c() = (tmp >> 16) != 0;

								// todo -- overflow!
								reg_a = tmp;
								reg_a.set_nz(flags);
							}
							break;

						case INC:
							dp_reg += 1;
							dp_reg.set_nz(flags);
							break;

						case DEC:
							dp_reg -= 1;
							dp_reg.set_nz(flags);
							break;

						case ASL:
							if (dp_reg) flags.c() = *dp_reg & 0x8000;
							dp_reg <<= 1;
							dp_reg.set_nz(flags);
							break;

						case LSR:
							if (dp_reg) flags.c() = *dp_reg & 0x0001;
							dp_reg >>= 1;
							dp_reg.set_nz(flags);
							break;

						case ROL:
							if (dp_reg && old_flags.c()) {
								flags.c() = dp_reg & 0x8000;
								dp_reg <<= 1;
								if (old_flags.c()) dp_reg |= 0x0001;
								dp_reg.set_nz(flags);

							}
							else {
								dp_reg = nullopt;
							}
							break;

						case ROR:
							if (dp_reg && old_flags.c()) {
								flags.c() = dp_reg & 0x0001;
								dp_reg >>= 1;
								if (old_flags.c()) dp_reg |= 0x8000;
								dp_reg.set_nz(flags);

							}
							else {
								dp_reg = nullopt;
							}
							break;



						case TSB:
							dp_reg |= reg_a;
							// z = reg & a == 0
							break;

						case TRB:
							dp_reg &= reg_a;
							break;

						case STA:
							dp_reg = reg_a;
							break;

						case STX:
							dp_reg = reg_x;
							break;

						case STY:
							dp_reg = reg_y;
							break;

						case STZ:
							dp_reg = (uint16_t)0;
							break;

						case AND:
							reg_a &= dp_reg;
							reg_a.set_nz(flags);
							break;

						case EOR:
							reg_a ^= dp_reg;
							reg_a.set_nz(flags);
							break;

						case ORA:
							reg_a |= dp_reg;
							reg_a.set_nz(flags);
							break;

						case LDA:
							if (reg_a == dp_reg) {
								kill = true;
								break;
							}

							reg_a = dp_reg;

							if (reg_x == reg_a) {
								BasicLinePtr tmp = BasicLine::Make(TXA, implied);
								tmp->calc_registers();
								out.emplace_back(std::move(tmp));
								continue;
							}

							if (reg_y == reg_a) {
								BasicLinePtr tmp = BasicLine::Make(TYA, implied);
								tmp->calc_registers();
								out.emplace_back(std::move(tmp));
								continue;
							}

							// also... replace lda <zp with lda #imm if known.

							break;
						case LDX:
							if (reg_x == dp_reg) kill = true;
							reg_x = dp_reg;
							break;
						case LDY:
							if (reg_y == dp_reg) kill = true;
							reg_y = dp_reg;
							break;

						default:
							if (op.writes_zp()) dp_reg = nullopt;
							if (op.writes_a()) reg_a = nullopt;
							if (op.writes_x()) reg_x = nullopt;
							if (op.writes_y()) reg_y = nullopt;
					}

					if (op.writes_zp() && dp_reg == old_dp)
						kill = true;


					if (mnemonic == PEI) {
						if (dp_reg) {
							line->opcode = OpCode(m65816, PEA, absolute);
							line->operands[0] = Expression::Integer(*dp_reg);
							line->calc_registers();

							out.push_back(line);
							continue;
						}
						if (dp_reg == reg_a) {
							// if PEI a dp_reg and the value is known to be in a, x, or y,
							// push it.  todo -- need to track x/y bits...
							// does this work if 
							BasicLinePtr tmp = BasicLine::Make(PHA, implied);
							tmp->calc_registers();
							out.emplace_back(std::move(tmp));
							continue;
						}

						if (dp_reg == reg_x) {
							BasicLinePtr tmp = BasicLine::Make(PHX, implied);
							tmp->calc_registers();
							out.emplace_back(std::move(tmp));
							continue;
						}

						if (dp_reg == reg_y) {
							BasicLinePtr tmp = BasicLine::Make(PHY, implied);
							tmp->calc_registers();
							out.emplace_back(std::move(tmp));
							continue;
						}

					}

					// replace lda <%t0 w/ lda #xxx if value known.
					if (!kill && dp_reg) {
						if (op.hasAddressMode(immediate)) {
							line->opcode = OpCode(m65816, mnemonic, immediate);
							line->operands[0] = Expression::Integer(*dp_reg);
							line->calc_registers();

							out.push_back(line);
							continue;							
						}
					}

					goto store;
				}
			}

			if (mode == implied || mode == immediate) {
				reg_value value(0);

				switch(mode) {
					case immediate:
						{
							uint32_t tmp;
							if (e->is_integer(tmp)) {
								value = (uint16_t)tmp;
							}
							else value = nullopt;
						}
						break;
					case implied:
						break;

					default:
						value = nullopt;
				}

				switch (mnemonic) {

					case REP:
						if (value) {
							if (*value & 0x20)
								reg_a = nullopt;
							if (*value & 0x30) {
								reg_x = nullopt;
								reg_y = nullopt;
							}
						}
						flags.rep(value);
						break;

					case SEP:
						// not true but good enough.
						if (value) {
							if (*value & 0x20)
								reg_a = nullopt;
							if (*value & 0x30) {
								reg_x = nullopt;
								reg_y = nullopt;
							}
						}
						flags.sep(value);
						break;

					case SEC:
						if (old_flags.c().value_or(0))
							kill = true;
						flags.c() = true;
						break;

					case CLC:
						if (!old_flags.c().value_or(1))
							kill = true;
						flags.c() = false;
						break;

					case SED:
						if (old_flags.d().value_or(0))
							kill = true;
						flags.d() = true;
						break;

					case CLD:
						if (!old_flags.d().value_or(1))
							kill = true;
						flags.d() = false;
						break;



					case LDA:
						if (reg_a == value) kill = true;
						reg_a = value;
						break;
					case LDX:
						if (reg_x == value) kill = true;
						reg_x = value;
						break;
					case LDY:
						if (reg_y == value) kill = true;
						reg_y = value;
						break;

					case INC:
						reg_a += 1;
						reg_a.set_nz(flags);
						break;

					case INX:
						reg_x += 1;
						reg_x.set_nz(flags);
						break;

					case INY:
						reg_y += 1;
						reg_y.set_nz(flags);
						break;

					case DEC:
						reg_a -= 1;
						reg_a.set_nz(flags);
						break;

					case DEX:
						reg_x -= 1;
						reg_x.set_nz(flags);
						break;

					case DEY:
						reg_y -= 1;
						reg_y.set_nz(flags);
						break;

					case TAX:
						if (reg_x == reg_a) kill = true;
						reg_x = reg_a;
						break;
					case TAY:
						if (reg_y == reg_a) kill = true;
						reg_y = reg_a;
						break;
					case TXA:
						if (reg_a == reg_x) kill = true;
						reg_a = reg_x;
						break;
					case TYA:
						if (reg_a == reg_y) kill = true;
						reg_a = reg_y;
						break;

					case XBA:
						if (reg_a) {
							reg_a = (*reg_a << 8) | (*reg_a >> 8);
						}
						break;

					default:
						if (op.writes_a()) reg_a = nullopt;
						if (op.writes_x()) reg_x = nullopt;
						if (op.writes_y()) reg_y = nullopt;
				}

				goto store;
			}


			if (op.writes_a()) reg_a = nullopt;
			if (op.writes_x()) reg_x = nullopt;
			if (op.writes_y()) reg_y = nullopt;
		}

store:

		if (kill) {
				delta = true;
		} else {
			out.push_back(line);
		}
	}

	list = std::move(out);

	return delta;
}

