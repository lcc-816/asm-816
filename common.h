
#ifndef __common_h__
#define __common_h__

#include <string>

#include "Machine.h"
#include "Instruction.h"

class Expression;

struct dp_register {
	unsigned type;
	unsigned number;
};


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

	// Line data..
	const std::string *label = nullptr;

	Instruction instruction;
	AddressMode mode = kUndefinedAddressMode;
	bool explicit_mode = false;

	Expression *operands[2] = {0, 0};

	// add these in later?
	unsigned instruction_size = 0;
	unsigned pc = 0;
	unsigned mx = 0;

	// other data... directive, etc?

	Line *next = nullptr;
};

struct Cookie {

	Line scratch;
	Line *head = nullptr;
	Line *tail = nullptr;

};

#endif
