#ifndef __cookie_h__
#define __cookie_h__

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "common.h"
#include "symbol_table.h"

#include "types.h"

#include "cxx/variant.h"



struct Token {

	Token() = delete;
	Token(const Token &) = default;
	Token(Token &&) = default;


	template <class T>
	Token(unsigned l, T &&t) : line(l), value(emplaced_type_t<T>{}, std::forward<T>(t))
	{}

	~Token() = default;


	unsigned line;
	variant<identifier, uint32_t, dp_register, Instruction, Mnemonic, ExpressionPtr, branch> value;

	identifier string_value() {
		return get<identifier>(value);
	}

	uint32_t int_value() {
		return get<uint32_t>(value);
	}

	inline dp_register register_value() {
		return get<dp_register>(value);
	}


	Mnemonic mnemonic_value() {
		return get<Mnemonic>(value);
	}

	Instruction instruction_value() {
		return get<Instruction>(value);
	}

	ExpressionPtr expr_value() {
		return get<ExpressionPtr>(value);
	}

	const branch branch_value() {
		return get<branch>(value);
	}
};


struct Cookie {

	enum SegmentType {
		none,
		code,
		data
	};


	unsigned line_number = 1;
	bool line_error = false;
	bool line_warning = false;

	bool syntax_error = false;
	bool parse_error = false;

	SegmentQueue segments;
	std::unique_ptr<Segment> data_segment; // anonymous data segment.
	
	// current segment.
	Segment *segment = nullptr;
	LineQueue *lines = nullptr;
	SegmentType seg_type = none;
	identifier current_label; 

	//

	std::unordered_set<identifier> export_set;
	std::unordered_set<identifier> import_set; // parsed but ignored.

	std::vector<identifier> strong_vector;

	std::unordered_map<identifier, unsigned> labels; // label -> line number.

	symbol_table equates;




	void add_label(identifier label);
	void begin_segment(identifier name, SegmentType type);
	void end_segment();


	// lexer stuff.

	struct {


		int cs;
		int act;
		const char * ts;
		const char * te;

		// for error reporting.
		const char * line_start = nullptr;
		const char * line_end = nullptr;

		int next_operand = 0;


	} fsm;
	
};

#endif
