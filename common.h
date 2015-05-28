
#ifndef __common_h__
#define __common_h__

#include <string>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include "Machine.h"
#include "Instruction.h"
#include "OpCode.h"
#include "register_set.h"

class Expression;


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




struct Token {
	unsigned line;
	union {
		const std::string *string_value;
		uint32_t int_value;
		dp_register register_value;
		Mnemonic mnemonic_value;
		Expression *expr_value;
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


enum {
	reg_none = 0,
	reg_read = 1,
	reg_write = 2,
	reg_rw = 3,
	reg_read_long = 5
};

struct BasicLine {
	// Line data while analyzing.

	const std::string *label = nullptr;
	Directive directive = kUndefinedDirective;

	OpCode opcode;

	Expression *operands[2] = {0, 0};

	bool m = false;
	bool x = false;
	bool long_branch = false;



	uint32_t pc = 0;
	// live registers, etc.

	register_set reg_live;
	dp_register reg; // from operands.
	unsigned reg_status = reg_none;
};
typedef std::deque<BasicLine *> LineQueue;

struct BasicBlock {

	unsigned size = 0;
	unsigned pc = 0;
	unsigned id = 0;

	const std::string *label = nullptr;

	LineQueue lines;


	register_set reg_import;
	register_set reg_export;

	bool processed = false;
	bool dead = false; // mark for dead-code elimination.
	bool entry = false; // this is an entry point (can't dead-code eliminate)

	std::vector<BasicBlock *> next_set;
	std::vector<BasicBlock *> prev_set;

};
typedef std::deque<BasicBlock *> BlockQueue;


struct Segment {
	const std::string *name = nullptr;
	const std::string *segment = nullptr;

	BlockQueue blocks;
	LineQueue lines;

	// attributes

	unsigned parm_size = 0;
	unsigned local_size = 0;
	unsigned temp_size = 0;
	unsigned return_size = 0;
	unsigned kind = 0;
	bool rts = false;

	enum {
		cdecl = 0,
		pascal,
		stdcall
	} convention = cdecl;
};

typedef std::deque< std::unique_ptr<Segment> > SegmentQueue;


bool peephole(LineQueue &);
void print(const LineQueue &lines);
void simplify(LineQueue &lines);
bool parse_file(const std::string &filename, SegmentQueue &segments);

unsigned classify(Mnemonic);
unsigned classify(OpCode);

LineQueue basic_block(LineQueue &&lines);

#endif
