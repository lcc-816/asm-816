#include <string>
#include <algorithm>
#include <numeric>
#include <unordered_map>

#include <cctype>
#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "Machine.h"
#include "Instruction.h"
#include "OpCode.h"
#include "Expression.h"
#include "common.h"
#include "grammar.h"
#include "intern.h"
#include "cookie.h"


namespace {


	int tox(char c)
	{
		c |= 0x20; // lowercase it.
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		return 0;
	}

	uint32_t scan10(const char *begin, const char *end)
	{
		return std::accumulate(begin, end, 0, 
			[](uint32_t value, char c){
				return value * 10 + c - '0';
			});
	}

	uint32_t scan16(const char *begin, const char *end)
	{
		return std::accumulate(begin, end, 0, 
			[](uint32_t value, char c){
				return (value << 4) + tox(c);
			});
	}

	uint32_t scan2(const char *begin, const char *end)
	{
		return std::accumulate(begin, end, 0, 
			[](uint32_t value, char c){
				return (value << 1) + (c & 0x01);
			});
	}

	uint32_t scancc(const char *begin, const char *end)
	{
		return std::accumulate(begin, end, 0,
			[](uint32_t value, char c)
			{
				return (value << 8) + (unsigned)c;
			}
		);

	}


	const char *line_start = nullptr;
	const char *eof = nullptr;
	unsigned line = 0;
	unsigned error_count = 0;
	unsigned warn_count = 0;

	int next_operand = 0;

}

void *ParseAlloc(void *(*mallocProc)(size_t));
void ParseFree(void *p, void (*freeProc)(void*));

void Parse(void *yyp, int yymajor, Token yyminor, Cookie *cookie);

void Parse(void *yyp, int yymajor, const std::string &string_value, Cookie *cookie)
{
	Token t;
	t.line = line;
	t.string_value = intern(string_value);
	Parse(yyp, yymajor, t, cookie);
}

void Parse(void *yyp, int yymajor, uint32_t int_value, Cookie *cookie)
{
	Token t;
	t.line = line;
	t.int_value = int_value;
	Parse(yyp, yymajor, t, cookie);
}


void Parse(void *yyp, int yymajor, dp_register register_value, Cookie *cookie)
{
	Token t;
	t.line = line;
	t.register_value = register_value;
	Parse(yyp, yymajor, t, cookie);
}

void Parse(void *yyp, int yymajor, Expression &expr_value, Cookie *cookie)
{
	Token t;
	t.line = line;
	t.expr_value = std::addressof(expr_value);
	Parse(yyp, yymajor, t, cookie);
}




%%{
	machine lexer;

	action eol {

			Parse(parser, tkEOL, 0, &cookie);
			Parse(parser, 0, 0, &cookie);

			line_start = te;
			cookie.line_number = ++line;

			next_operand = 0;
			fgoto main;
	}

	action error {
			fprintf(stderr, "Unable to lex!\n");
			// clear out the parser
			Parse(parser, 0, 0, &cookie);
			fgoto error;
	}

	action parse_identifier {
			std::string s(ts, te);
			Parse(parser, tkIDENTIFIER, s, &cookie);	
	}

	identifier = [A-Za-z_][A-Za-z0-9_$]*;

	comment := |*
		'\r'|'\n'|'\r\n' => eol;

		any {};
	*|;

	error := |*

		'\r'|'\n'|'\r\n' {
			// slightly modified version of eol.
			//Parse(parser, 0, 0, &cookie); happens before we enter.
	
			line++;
			line_start = te;

			next_operand = 0;
			fgoto main;
		};

		any {};
	*|;



	operand := |*

		# white space
		'\r'|'\n'|'\r\n' => eol;

		[\t ] {};

		# comments


		';' {
			fgoto comment;
		};

		# single-char ops
		'(' { Parse(parser, tkLPAREN, 0, &cookie); };
		')' { Parse(parser, tkRPAREN, 0, &cookie); };
		'[' { Parse(parser, tkLBRACKET, 0, &cookie); };
		']' { Parse(parser, tkRBRACKET, 0, &cookie); };
		'=' { Parse(parser, tkEQ, 0, &cookie); };
		'+' { Parse(parser, tkPLUS, 0, &cookie); };
		'-' { Parse(parser, tkMINUS, 0, &cookie); };
		'*' { Parse(parser, tkSTAR, 0, &cookie); };
		'/' { Parse(parser, tkSLASH, 0, &cookie); };
		'%' { Parse(parser, tkPERCENT, 0, &cookie); };
		'~' { Parse(parser, tkTILDE, 0, &cookie); };
		'!' { Parse(parser, tkBANG, 0, &cookie); };
		'^' { Parse(parser, tkCARET, 0, &cookie); };
		'&' { Parse(parser, tkAMP, 0, &cookie); };
		'|' { Parse(parser, tkPIPE, 0, &cookie); };
		'<' { Parse(parser, tkLT, 0, &cookie); };
		'>' { Parse(parser, tkGT, 0, &cookie); };
		'|' { Parse(parser, tkPIPE, 0, &cookie); };
		'#' { Parse(parser, tkHASH, 0, &cookie); };
		':' { Parse(parser, tkCOLON, 0, &cookie); };
		#'.' { Parse(parser, tkPERIOD, 0, &cookie); };
		',' { Parse(parser, tkCOMMA, 0, &cookie); };

		'*' { Parse(parser, tkSTAR, 0, &cookie); };

		# dp-registers -- %t0, etc
		'%' [prtv] digit+ {
			unsigned type = ts[1];
			unsigned number = scan10(ts+2, te);
			dp_register dp = {type, number };
			Parse(parser, tkDP_REGISTER, dp, &cookie);
		};

		# real registers
		#'a'i { Parse(parser, tkREGISTER_A, 0, &cookie); };
		'x'i { Parse(parser, tkREGISTER_X, 0, &cookie); };
		'y'i { Parse(parser, tkREGISTER_Y, 0, &cookie); };
		'z'i { Parse(parser, tkREGISTER_Z, 0, &cookie); };
		's'i { Parse(parser, tkREGISTER_S, 0, &cookie); };

		# numbers
		'$' xdigit + {
			uint32_t value = scan16(ts + 1, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'0x'i xdigit+ {
			// hexadecimal
			uint32_t value = scan16(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'0b'i [01]+ {
			// binary
			uint32_t value = scan2(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'%' [01]+ {
			// binary
			uint32_t value = scan2(ts + 1, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		digit+ {
			uint32_t value = scan10(ts, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		['] [^']{1,4} ['] {
			// 4 cc code

			uint32_t value = scancc(ts + 1, te - 1);
			Parse(parser, tkINTEGER, value, &cookie);

		};

		# strings.
		["] ([^"])* ["] {
			std::string s(ts + 1, te - 1);
			Parse(parser, tkSTRING, s, &cookie);
		};

		#identifier => parse_identifier;

		identifier => {
			// may be an equate...
			std::string s(ts, te);
			auto id = intern(s);

			ExpressionPtr e = cookie.equates.find(id);
			if (e) {
				dp_register dp;
				// register is a special case...

				if (e->is_register(dp)) {
					Parse(parser, tkDP_REGISTER, dp, &cookie);
				}
				else Parse(parser, tkEXPRESSION, *e, &cookie);
			} else {
				Parse(parser, tkIDENTIFIER, s, &cookie);
			}

		};

		any => error;

	*|;


	operand_no_reg := |*

		# white space
		'\r'|'\n'|'\r\n' => eol;

		[\t ] {};

		# comments


		';' {
			fgoto comment;
		};

		# single-char ops
		'(' { Parse(parser, tkLPAREN, 0, &cookie); };
		')' { Parse(parser, tkRPAREN, 0, &cookie); };
		'[' { Parse(parser, tkLBRACKET, 0, &cookie); };
		']' { Parse(parser, tkRBRACKET, 0, &cookie); };
		'=' { Parse(parser, tkEQ, 0, &cookie); };
		'+' { Parse(parser, tkPLUS, 0, &cookie); };
		'-' { Parse(parser, tkMINUS, 0, &cookie); };
		'*' { Parse(parser, tkSTAR, 0, &cookie); };
		'/' { Parse(parser, tkSLASH, 0, &cookie); };
		'%' { Parse(parser, tkPERCENT, 0, &cookie); };
		'~' { Parse(parser, tkTILDE, 0, &cookie); };
		'!' { Parse(parser, tkBANG, 0, &cookie); };
		'^' { Parse(parser, tkCARET, 0, &cookie); };
		'&' { Parse(parser, tkAMP, 0, &cookie); };
		'|' { Parse(parser, tkPIPE, 0, &cookie); };
		'<' { Parse(parser, tkLT, 0, &cookie); };
		'>' { Parse(parser, tkGT, 0, &cookie); };
		'|' { Parse(parser, tkPIPE, 0, &cookie); };
		'#' { Parse(parser, tkHASH, 0, &cookie); };
		':' { Parse(parser, tkCOLON, 0, &cookie); };
		#'.' { Parse(parser, tkPERIOD, 0, &cookie); };
		',' { Parse(parser, tkCOMMA, 0, &cookie); };

		'*' { Parse(parser, tkSTAR, 0, &cookie); };

		'<<' { Parse(parser, tkLTLT, 0, &cookie); };
		'>>' { Parse(parser, tkGTGT, 0, &cookie); };

		# numbers
		'$' xdigit + {
			uint32_t value = scan16(ts + 1, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'0x'i xdigit+ {
			// hexadecimal
			uint32_t value = scan16(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'0b'i [01]+ {
			// binary
			uint32_t value = scan2(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'%' [01]+ {
			// binary
			uint32_t value = scan2(ts + 1, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		digit+ {
			uint32_t value = scan10(ts, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		['] [^']{1,4} ['] {
			// 4 cc code

			uint32_t value = scancc(ts + 1, te - 1);
			Parse(parser, tkINTEGER, value, &cookie);

		};

		# strings.
		["] ([^"])* ["] {
			std::string s(ts, te);
			Parse(parser, tkSTRING, s, &cookie);
		};

		identifier => parse_identifier;

		any => error;

	*|;


	operand_pragma := |*

		# white space
		'\r'|'\n'|'\r\n' => eol;

		[\t ]+ {};

		';' { fgoto comment; };

		'=' { Parse(parser, tkEQ, 0, &cookie); };
		',' { Parse(parser, tkCOMMA, 0, &cookie); };

		'segment'i {
			Parse(parser, tkSEGMENT, 0, &cookie);
		};

		'private'i {
			Parse(parser, tkPRIVATE, 0, &cookie);
		};

		'kind'i {
			Parse(parser, tkKIND, 0, &cookie);
		};

		'cdecl'i {
			Parse(parser, tkCDECL, 0, &cookie);
		};
		'pascal'i {
			Parse(parser, tkPASCAL, 0, &cookie);
		};
		'stdcall'i {
			Parse(parser, tkSTDCALL, 0, &cookie);
		};

		'void'i {
			Parse(parser, tkVOID, 0, &cookie);
		};

		'rts'i {
			Parse(parser, tkRTS, 0, &cookie);
		};

		'locals'i {
			Parse(parser, tkLOCALS, 0, &cookie);
		};

		'parameters'i {
			Parse(parser, tkPARAMETERS, 0, &cookie);
		};

		'return'i {
			Parse(parser, tkRETURN, 0, &cookie);
		};

		# numbers
		'$' xdigit + {
			uint32_t value = scan16(ts + 1, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'0x'i xdigit+ {
			// hexadecimal
			uint32_t value = scan16(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'0b'i [01]+ {
			// binary
			uint32_t value = scan2(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'%' [01]+ {
			// binary
			uint32_t value = scan2(ts + 1, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		digit+ {
			uint32_t value = scan10(ts, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		
		any => error;
	*|;




	opcode := |*

		# white space
		'\r'|'\n'|'\r\n' => eol;

		[\t ]+ { 
			if (next_operand) fgoto *next_operand; 
			else fgoto operand; 
		};

		';' { fgoto comment; };



		'dc.b'i {
			Parse(parser, tkDCB, 0, &cookie);
			next_operand = lexer_en_operand_no_reg;
		};

		'dc.w'i {
			Parse(parser, tkDCW, 0, &cookie);
			next_operand = lexer_en_operand_no_reg;
		};

		'dc.l'i {
			Parse(parser, tkDCL, 0, &cookie);
			next_operand = lexer_en_operand_no_reg;
		};

		'ds'i {
			Parse(parser, tkDS, 0, &cookie);
			next_operand = lexer_en_operand_no_reg;
		};

		'pragma'i {
			Parse(parser, tkPRAGMA, 0, &cookie);
			next_operand = lexer_en_operand_pragma;
		};

		'equ'i {
			Parse(parser, tkEQU, 0, &cookie);
		};

		# create a separate context for first token vs
		# checking whitespace?
		'start'i {
			Parse(parser, tkSTART, 0, &cookie);
		};

		'data'i {
			Parse(parser, tkDATA, 0, &cookie);
		};

		'end'i {
			Parse(parser, tkEND, 0, &cookie);
		};

		'export'i {
			Parse(parser, tkEXPORT, 0, &cookie);
			next_operand = lexer_en_operand_no_reg;
		};

		#'begin_stack'i {
		#	Parse(parser, tkPROLOGUE, 0, &cookie);
		#};

		#'end_stack'i {
		#	Parse(parser, tkEPILOGUE, 0, &cookie);
		#};

		[A-Za-z_][A-Za-z0-9_]* {

			// mvp or mvn generate tkOPCODE_2.
			// all others generate tkOPCODE.
			// check opcode table

			std::string s(ts, te);
			Instruction instr(m65816, s);
			if (instr) {
				// just insert the instruction directly into the cookie.
				// since it's easier than dealing with c++ unions.

				cookie.scratch.instruction = instr;
				unsigned tk = tkOPCODE;
				if (instr.hasAddressMode(block)) tk = tkOPCODE_2;
				if (instr.hasAddressMode(zp_relative)) tk = tkOPCODE_2;
				Parse(parser, tk, 0, &cookie);
				//fgoto operand;

			} else {
				error("Invalid opcode/directive");
				Parse(parser, 0, 0, &cookie);
				fgoto error;
			}
		};

		any => error;
	*|;




	main := |*
		# white space
		'\r'|'\n'|'\r\n' => eol;

		[\t ]+ { fgoto opcode; };

		# comments
		'*' { fgoto comment; };

		';' { fgoto comment; };

		identifier => parse_identifier;


		# identifier
		#'@' [A-Za-z0-9_]+ {
		#	// @ is appended to the current label.
		#};

		# this is here so we can have label:
		# maybe we should just drop the :
		':' { Parse(parser, tkCOLON, 0, &cookie); };

		any {
			fprintf(stderr, "Unable to lex!\n");
			// clear out the parser
			Parse(parser, 0, 0, &cookie);
			fgoto error;
		};

	*|;
}%%


%% write data;

void error(const std::string &s) {

	fprintf(stderr, "Error: Line %u: %s\n", line, s.c_str());

	const char *p = line_start;
	while (p != eof) {
		char c = *p++;
		if (c == '\r' || c == '\n') break;
		fputc(c, stderr);
	}
	fprintf(stderr, "\n");


	error_count++;
}

void warn(const std::string &s) {

	fprintf(stderr, "Error: Line %u: %s\n", line, s.c_str());

	const char *p = line_start;
	while (p != eof) {
		char c = *p++;
		if (c == '\r' || c == '\n') break;
		fputc(c, stderr);
	}
	fprintf(stderr, "\n");


	warn_count++;
}

bool parse_file(const std::string &filename, std::deque<BasicLine *> &rv)
{
	int fd;
	struct stat st;
	void *buffer;
	int ok;


	Cookie cookie;


	const std::string *prevLabel;
	void *parser;

	fd = open(filename.c_str(), O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Unable to open file `%s' : %s\n", filename.c_str(), strerror(errno));
		return false;
	}

	ok = fstat(fd, &st);
	if (ok < 0) {
		fprintf(stderr, "Unable to fstat file `%s' : %s\n", filename.c_str(), strerror(errno));
		close(fd);
		return false;
	}

	buffer = mmap(nullptr, st.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
	if (buffer == MAP_FAILED) {
		fprintf(stderr, "Unable to mmap file `%s' : %s\n", filename.c_str(), strerror(errno));
		close(fd);
		return false;
	}
	close(fd);

	parser = ParseAlloc(malloc);

	const char *p = (const char *)buffer;
	const char *pe = (const char *)buffer + st.st_size;
	//const char *eof = pe;
	eof = pe;
	const char *ts;
	const char *te;
	int cs, act;

	line_start = p;
	cookie.line_number = line = 1;

	%% write init;
	//
	%% write exec;

	Parse(parser, tkEOL, 0, &cookie);
	Parse(parser, 0, 0, &cookie);
	ParseFree(parser, free);

	munmap(buffer, st.st_size);
	if (error_count > 0) {
		for (BasicLine *tmp : cookie.lines) {
			delete tmp;
		}
		cookie.lines.clear();
		return false;
	}
	rv = std::move(cookie.lines);
	return true;
}