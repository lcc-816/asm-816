
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
#include "dp_register_set.h"
#include "register_set.h"
#include "branch.h"

#include "types.h"
#include "cxx/filesystem.h"


enum Directive {
	kUndefinedDirective = 0,
	DCB,
	DCW,
	DCL,
	DS,
	ALIGN,
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
		operands[0] = std::forward<E>(a);
	}

	template<class E>
	BasicLine(Mnemonic m, AddressMode mode, E && a, E && b) {
		opcode = OpCode(m65816, m, mode);
		operands[0] = std::forward<E>(a);
		operands[1] = std::forward<E>(b);
	}

	template<class E>
	BasicLine(const OpCode &op, E &&a) {
		opcode = op;
		operands[0] = std::forward<E>(a);
	}

	template<class E>
	BasicLine(const OpCode &op, E && a, E && b) {
		opcode = op;
		operands[0] = std::forward<E>(a);
		operands[1] = std::forward<E>(b);
	}

	BasicLine(Directive d) {
		directive = d;
	}

	template<class E>
	BasicLine(Directive d, E &&e) {
		directive = d;
		operands[0] = std::forward<E>(e);
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

	//dp_register_set reg_live;
	dp_register reg; // from operands.
	unsigned reg_count = 0; 
	unsigned reg_status = reg_none;


	// update reg, reg_count, reg_status.
	// depends on opcode, operands, longM, longX
	void calc_registers();
	register_set read_registers() const;
	register_set write_registers() const;
};


typedef std::deque<BasicLinePtr> LineQueue;

typedef std::deque<BasicBlockPtr> BlockQueue;

#include "basic_block.h"

class Segment {
public:
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
	bool global = false;
	bool databank = false;
	bool noreturn = false;
	bool has_volatile = false;


	enum {
		rtl = 0,
		rts,
		rti
	} return_type = rtl;

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
	std::vector<identifier> strong_vector;

};

// n. b. -- other stuff outside of segment, like export / import set.
typedef std::deque< SegmentPtr> SegmentQueue;


// Translation unit aka file
struct Module {
	std::string filename;
	SegmentQueue segments;
	std::vector<identifier> imports;
	std::vector<identifier> exports;
};

typedef std::unique_ptr<Module> ModulePtr;

std::unique_ptr<Module> parse_file(int fd);
std::unique_ptr<Module> parse_file(const filesystem::path &p);


bool peephole(BasicBlockPtr);
bool final_peephole(BasicBlockPtr);

void print(const LineQueue &lines);
void simplify(LineQueue &lines);

bool parse_file(const filesystem::path &path, SegmentQueue &segments);
bool parse_file(FILE *file, SegmentQueue &segments);

unsigned classify(Mnemonic);
unsigned classify(OpCode);

void basic_block(Segment *segment);

#endif
