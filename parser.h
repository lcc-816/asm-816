#ifndef __parser_h__
#define __parser_h__


#include <lemon_base.h>
#include <vector>
#include <string>

#include "types.h"
#include "token.h"
#include "symbol_table.h"
#include "common.h"

class parser : public lemon_base<Token> {

public:

	enum SegmentType {
		none,
		code,
		data,
		record,
		macro
	};

	static std::unique_ptr<parser> make();

	//using lemon_base::parse;

	SegmentQueue segments();
	std::vector<identifier> imports();
	std::vector<identifier> exports();


	void error(const std::string &);
	void warn(const std::string &);

	void error(const Token &t, const std::string &);
	void warn(const Token &t, const std::string &);

/*
	void error(const Token &t, const char *format, ...);
	void warn(const Token &t, const char *format, ...);
*/

	int error_count() const { return _error; }
	int warn_count() const { return _warn; }

	void parse_token(Token &&);
	void drain_queue();

	std::unique_ptr<Module> module();

protected:



	identifier expand_at(identifier id) const;

	void add_label(identifier label, bool hidden = false);
	void add_label(const Token &t, bool hidden = false);

	void add_line(BasicLinePtr &&line);
	void add_lines(std::vector<BasicLinePtr> &&line);

	void begin_segment(identifier name, SegmentType type);
	void end_segment();

	identifier gen_sym();

	virtual void syntax_error(int yymajor, token_type &yyminor) override;
	virtual void parse_failure() override;
	void expand_include(const Token &t, const filesystem::path &p);
	void expand_macro(const Token &t, std::vector<Token> &&arguments);

	bool check_macro(Token &t);
	bool check_opcode(Token &t);
	bool check_equate(Token &t);



	std::deque<Token> _queue;
	std::unordered_set<identifier> _export_set;
	std::unordered_set<identifier> _import_set;
	std::unordered_set<identifier> _labels; // labels in the current segment.

	// records...
	symbol_table _equates;


	SegmentQueue _segments;
	std::unique_ptr<Segment> _data_segment; // anonymous data segment.
	
	// current segment.
	Segment *_segment = nullptr;
	LineQueue _lines;
	SegmentType _seg_type = none;

	unsigned _error = 0;
	unsigned _warn = 0;
	unsigned _gen_sym = 0;


	std::vector<identifier> _file_stack;
	identifier _current_file = nullptr;

	std::vector<identifier> _macro_stack;
	identifier _current_macro = nullptr;

	identifier _current_label = nullptr;




	/* opcode for current line.  used by error/warn. */
	Token _opcode = {};

protected:
	/*
	 * macro stuff
	 */


	bool install_macro(const Token &t, const std::vector<Token> &formal);
	bool install_equate(const Token &t, ExpressionPtr &e);

	struct macro {
		identifier name = nullptr;
		identifier file = nullptr;
		int line = 0;
		bool variadic = false; // true if last parm is &xxx....
		std::vector<identifier> params;
		std::vector<Token> body;
	};
	macro _tmp_macro;
	std::unordered_map<identifier, macro> _macros;

};



#endif

