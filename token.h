#ifndef __token_h__
#define __token_h__

#include <cstdint>
#include "include/variant.h"

#include "types.h"
#include "dp_register.h"
#include "Instruction.h"
#include "branch.h"

struct Token {

	Token() = default;
	Token(const Token &) = default;
	Token(Token &&) = default;

	Token(unsigned l, identifier i) : line(l), id(i)
	{}


	template <class T>
	Token(unsigned l, T &&t) : line(l), value(std::forward<T>(t))
	{}

	template <class T>
	Token(unsigned l, identifier i, T &&t) : line(l), id(i), value(std::forward<T>(t))
	{}


	~Token() = default;


	Token& operator=(const Token &) = default;
	Token& operator=(Token &&) = default;


	identifier id = nullptr;
	identifier file = nullptr;
	unsigned type = 0;
	unsigned line = 0;


	variant<uint32_t, dp_register, Instruction, Mnemonic, ExpressionPtr, branch::branch_type> value;

	const std::string &string_value() const noexcept {
		static std::string empty;
		return id ? *id : empty;
	}

	uint32_t int_value() const {
		return get<uint32_t>(value);
	}

	inline dp_register register_value() const {
		return get<dp_register>(value);
	}


	Mnemonic mnemonic_value() const {
		return get<Mnemonic>(value);
	}

	Instruction instruction_value() const {
		return get<Instruction>(value);
	}

	ExpressionPtr expr_value() const {
		return get<ExpressionPtr>(value);
	}

	const branch::branch_type branch_value() const {
		return get<branch::branch_type>(value);
	}
};

#endif
