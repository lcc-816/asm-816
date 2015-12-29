
#include "common.h"
#include "Instruction.h"
#include "Machine.h"
#include "Expression.h"
#include "OpCode.h"

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
		_data = value;
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

bool reg_const(LineQueue &list) {



	//typedef optional<uint16_t> register_type;
	typedef reg_value register_type;


	std::unordered_map<dp_register, register_type> dp_table;


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

		if (line->directive == SMART_BRANCH && line->branch.writes_a()) {
			// branch could clobber a ... but it will also be at the end of the block,
			reg_a = nullopt;

			out.push_back(line);
			continue;
		}

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



			if (mnemonic == JSL || mnemonic == JSR) {
				// invalidate all %p and %v variables
				erase_if(dp_table, [](const dp_register &k, const reg_value &v){
					return !k.is_temporary();
				});
				/*
				for (auto it = dp_table.begin(); it != dp_table.end(); ) {
					if (it->first.is_temporary()) ++it;
					else it = dp_table.erase(it);
				}
				*/
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
						case INC:
							dp_reg += 1;
							break;

						case DEC:
							dp_reg -= 1;
							break;

						case TSB:
							dp_reg |= reg_a;
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
							break;

						case EOR:
							reg_a ^= dp_reg;
							break;

						case ORA:
							reg_a |= dp_reg;
							break;

						case LDA:
							if (reg_a == dp_reg) kill = true;
							// also... replace lda <zp with lda #imm if known.
							reg_a = dp_reg;
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
						break;

					case INX:
						reg_x += 1;
						break;

					case INY:
						reg_y += 1;
						break;

					case DEC:
						reg_a -= 1;
						break;

					case DEX:
						reg_x -= 1;
						break;

					case DEY:
						reg_y -= 1;
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

