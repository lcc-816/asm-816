
#ifndef __common_h__
#define __common_h__

#include <string>
#include <deque>

#include "Machine.h"
#include "Instruction.h"
#include "OpCode.h"

class Expression;

struct dp_register {
	unsigned type;
	unsigned number;

	operator bool() const { return (bool)type; }
};

enum Directive {
	kUndefinedDirective = 0,
	START,
	END,
	DATA,
	EXPORT,
	PRAGMA,
	DCB,
	DCW,
	DCL,
	DS,
	PROLOGUE,
	EPILOGUE,
};

inline bool operator==(const dp_register &a, const dp_register &b) {
	return a.type == b.type && a.number == b.number;
}

inline dp_register operator+(dp_register r, int i) {
	r.number += i;
	return r;
}


struct Token {
	unsigned line;
	union {
		const std::string *string_value;
		uint32_t int_value;
		dp_register register_value;
		Mnemonic mnemonic_value;
	};
};

struct Line {

	// Line data while parsing.
	const std::string *label = nullptr;
	Directive directive = kUndefinedDirective;

	Instruction instruction;
	AddressMode mode = kUndefinedAddressMode;
	bool explicit_mode = false;
	bool error = false;

	Expression *operands[2] = {0, 0};
};


struct BasicLine {
	// Line data while analyzing.

	const std::string *label = nullptr;
	Directive directive = kUndefinedDirective;

	OpCode opcode;

	Expression *operands[2] = {0, 0};

	uint32_t pc = 0;
	// live registers, etc.
};

typedef std::deque<BasicLine *> LineQueue;


struct Cookie {

	Line scratch;

	std::deque<BasicLine *> lines;
};

#endif
