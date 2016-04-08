#ifndef __cookie_h__
#define __cookie_h__

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "common.h"
#include "symbol_table.h"

#include "types.h"

#include "cxx/variant.h"

#include "lemon_base.h"


struct Token {

	Token() = delete;
	Token(const Token &) = default;
	Token(Token &&) = default;

	Token(unsigned l, identifier i) : line(l), id(i)
	{}


	template <class T>
	Token(unsigned l, T &&t) : line(l), value(emplaced_type_t<T>{}, std::forward<T>(t))
	{}

	template <class T>
	Token(unsigned l, identifier i, T &&t) : line(l), id(i), value(emplaced_type_t<T>{}, std::forward<T>(t))
	{}


	~Token() = default;


	unsigned line = 0;
	identifier id = nullptr;

	variant<uint32_t, dp_register, Instruction, Mnemonic, ExpressionPtr, branch::branch_type> value;

	identifier string_value() {
		return id;
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

	const branch::branch_type branch_value() {
		return get<branch::branch_type>(value);
	}
};


class Parser : public lemon_base<Token>
{
public:

	static std::unique_ptr<Parser> make();
	Parser();

	enum SegmentType {
		none,
		code,
		data
	};

	using lemon_base::parse;

	void parse(int yymajor, const std::string &string_value);
	void parse(int yymajor, uint32_t value);
	void parse(int yymajor, dp_register value);
	void parse(int yymajor, ExpressionPtr value);
	void parse(int yymajor, const std::string &string_value, const Instruction &value);
	void parse(int yymajor, const std::string &string_value, branch::branch_type value);



	void add_label(identifier label);
	void begin_segment(identifier name, SegmentType type);
	void end_segment();


	void error(const std::string &);
	void warn(const std::string &);

	void parse_text(const char *p, const char *pe, bool end_of_file);
	void get_segments(SegmentQueue &rv);


	unsigned error_count = 0;
	unsigned warn_count = 0;


protected:


	virtual void parse_failure() override {
		error("parse error");
		has_parse_error = true;
	}

	virtual void syntax_error(int yymajor, token_type &yyminor) override;


	unsigned line_number = 1;
	bool line_error = false;
	bool line_warning = false;

	bool has_syntax_error = false;
	bool has_parse_error = false;

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

	std::unordered_map<identifier, unsigned> labels; // label -> line number.

	symbol_table equates;


private:
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
