
#ifndef __common_h__
#define __common_h__

#include <string>

#include "Machine.h"
#include "Instruction.h"

class Expression;

struct dp_register {
	char type = 0;
	unsigned value = 0;
};



union Token {
	const std::string *string_value = 0;
	uint32_t int_value;
	dp_register register_value;
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
	unsigned mx = 0;

	// other data... directive, etc?

	Line *next = nullptr;
};


#endif