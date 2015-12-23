
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
#include "branch.h"

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
	ALIGN,
	PROLOGUE,
	EPILOGUE,
	SMART_BRANCH,
};




struct Token {
	unsigned line;
	union {
		const std::string *string_value;
		uint32_t int_value;
		dp_register register_value;
		Mnemonic mnemonic_value;
		Expression *expr_value;
		branch branch_value;
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
	branch branch;

	Expression *operands[2] = {0, 0};
};


enum {
	reg_none = 0,
	reg_read = 1,
	reg_write = 2,
	reg_rw = 3
};

struct BasicLine {
	// Line data while analyzing.

	BasicLine() = default;
	BasicLine(const BasicLine &) = default;
	BasicLine(BasicLine &&) = default;

	BasicLine(Mnemonic m, AddressMode mode, ExpressionPtr e = nullptr) {
		opcode = OpCode(m65816, m, mode);
		operands[0] = e;
	}

	BasicLine(Directive d, ExpressionPtr e = nullptr) {
		directive = d;
		operands[0] = e;
	}

	const std::string *label = nullptr;
	bool global = false;

	Directive directive = kUndefinedDirective;

	OpCode opcode;

	Expression *operands[2] = {0, 0};

	bool longM = true;
	bool longX = true;
	//bool long_branch = false;
	branch branch;


	uint32_t pc = 0;
	// live registers, etc.

	register_set reg_live;
	dp_register reg = { 0, 0 }; // from operands.
	unsigned reg_count = 0; 
	unsigned reg_status = reg_none;


	// update reg, reg_count, reg_status.
	// depends on opcode, operands, longM, longX
	void calc_registers();
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
	bool entry_node = false; // this is an entry point (can't dead-code eliminate)
	bool exit_node = false;

	std::vector<BasicBlock *> next_set;
	std::vector<BasicBlock *> prev_set;

};
typedef std::deque<BasicBlock *> BlockQueue;


struct Segment {
	const std::string *name = nullptr;
	const std::string *segment = nullptr;

	//BlockQueue blocks;
	LineQueue lines;

	// attributes

	unsigned parm_size = 0;
	unsigned local_size = 0;
	unsigned temp_size = 0;
	unsigned return_size = 0;
	unsigned kind = 0;
	bool dynamic = false;
	bool debug = false;
	bool rts = false;
	bool global = false;
	bool databank = false;

	enum {
		naked = 0,
		data,
		bss,
		arrays,
		cdecl,
		pascal,
		stdcall
	} convention = naked;

	std::vector<BasicLine *> prologue_code;
	std::vector<BasicLine *> epilogue_code;
};

// n. b. -- other stuff outside of segment, like export / import set.
typedef std::deque< std::unique_ptr<Segment> > SegmentQueue;


// Translation unit aka file
struct Unit {
	std::string filename;
	SegmentQueue segments;
	std::vector<identifier> imports;
	std::vector<identifier> exports;
};

bool peephole(LineQueue &);
void print(const LineQueue &lines);
void simplify(LineQueue &lines);
bool parse_file(const std::string &filename, SegmentQueue &segments);

unsigned classify(Mnemonic);
unsigned classify(OpCode);

void basic_block(Segment *segment);

#endif
