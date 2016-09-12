#ifndef __cookie_h__
#define __cookie_h__

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <utility>

#include "common.h"
#include "symbol_table.h"

#include "types.h"


#include "token.h"
#include "lemon_base.h"


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



	void add_label(identifier label, bool hidden = false);
	void add_line(BasicLinePtr &&line);
	void add_lines(std::vector<BasicLinePtr> &&line);

	void begin_segment(identifier name, SegmentType type);
	void end_segment();

	identifier gen_sym();


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
	LineQueue lines;
	SegmentType seg_type = none;
	identifier current_label; 

	//

	std::unordered_set<identifier> export_set;
	std::unordered_set<identifier> import_set; // parsed but ignored.

	std::unordered_map<identifier, unsigned> labels; // label -> line number.

	symbol_table equates;


private:
	// lexer stuff.

	unsigned _gen_sym = 0;

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
