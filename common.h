
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
#include "register_bits.h"
#include "branch.h"

#include "types.h"


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


	BasicLine(identifier l) : label(l)
	{}

	BasicLine(Mnemonic m, AddressMode mode) {
		opcode = OpCode(m65816, m, mode);
	}

	template<class E>
	BasicLine(Mnemonic m, AddressMode mode, E && a) {
		opcode = OpCode(m65816, m, mode);
		operands[0] = std::move(a);
	}

	template<class E>
	BasicLine(Mnemonic m, AddressMode mode, E && a, E && b) {
		opcode = OpCode(m65816, m, mode);
		operands[0] = std::move(a);
		operands[1] = std::move(b);
	}

	template<class E>
	BasicLine(const OpCode &op, E &&a) {
		opcode = op;
		operands[0] = std::move(a);
	}

	template<class E>
	BasicLine(const OpCode &op, E && a, E && b) {
		opcode = op;
		operands[0] = std::move(a);
		operands[1] = std::move(b);
	}

	BasicLine(Directive d) {
		directive = d;
	}

	template<class E>
	BasicLine(Directive d, E &&e) {
		directive = d;
		operands[0] = std::move(e);
	}



	template<class...Args>
	static BasicLinePtr Make(Args&&... args) {
		return std::make_shared<BasicLine>(std::forward<Args>(args)...);
	}

	identifier label = nullptr;
	bool global = false;

	Directive directive = kUndefinedDirective;

	OpCode opcode;

	ExpressionPtr operands[2];

	bool longM = true;
	bool longX = true;
	branch branch;


	uint32_t pc = 0;
	// live registers, etc.

	//register_set reg_live;
	dp_register reg; // from operands.
	unsigned reg_count = 0; 
	unsigned reg_status = reg_none;


	// update reg, reg_count, reg_status.
	// depends on opcode, operands, longM, longX
	void calc_registers();
	register_bits read_registers() const;
	register_bits write_registers() const;
};


typedef std::deque<BasicLinePtr> LineQueue;

typedef std::deque<BasicBlockPtr> BlockQueue;

class BasicBlock : public std::enable_shared_from_this<BasicBlock> {

public:

	static BlockQueue MakeBlockQueue(LineQueue &&);

	template<class...Args>
	static BasicBlockPtr Make(Args&&... args) {
		return std::make_shared<BasicBlock>(std::forward<Args>(args)...);
	}

	void recalc_next_set();
	void make_dead();

	void remove_prev(BasicBlockPtr);
	void remove_next(BasicBlockPtr);
	void replace_prev(BasicBlockPtr oldBlock, BasicBlockPtr newBlock);
	void replace_next(BasicBlockPtr oldBlock, BasicBlockPtr newBlock);


	unsigned pc = 0;
	//unsigned size = 0;
	//unsigned id = 0;

	identifier label = nullptr;

	LineQueue lines;
	BasicLinePtr exit_branch; // if ends w/ a branch.
	BasicBlockPtr next_block; // if fallthrough.



	register_set reg_import;
	register_set reg_export;

	bool processed = false;
	bool dead = false; // mark for dead-code elimination.
	bool entry_node = false; // this is an entry point (can't dead-code eliminate)
	bool exit_node = false;

	std::vector<BasicBlockPtr> next_set;
	std::vector<BasicBlockPtr> prev_set;

};



struct Segment {
	identifier name = nullptr;
	identifier segment = nullptr;

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

	std::vector<BasicLinePtr> prologue_code;
	std::vector<BasicLinePtr> epilogue_code;
};

// n. b. -- other stuff outside of segment, like export / import set.
typedef std::deque< SegmentPtr> SegmentQueue;


// Translation unit aka file
struct Unit {
	std::string filename;
	SegmentQueue segments;
	std::vector<identifier> imports;
	std::vector<identifier> exports;
};

bool peephole(BasicBlockPtr);
bool final_peephole(BasicBlockPtr);

void print(const LineQueue &lines);
void simplify(LineQueue &lines);
bool parse_file(const std::string &filename, SegmentQueue &segments);

unsigned classify(Mnemonic);
unsigned classify(OpCode);

void basic_block(Segment *segment);

#endif
