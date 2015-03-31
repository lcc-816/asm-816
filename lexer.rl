#include <string>
#include <algorithm>
#include <numeric>
#include <unordered_map>

#include <cctype>
#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "Machine.h"
#include "Instruction.h"
#include "Opcode.h"
#include "Expression.h"
#include "common.h"
#include "grammar.h"




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

	const std::string *intern(const std::string &s) {

		static std::unordered_multimap<size_t, std::string *> ht;
		std::hash<std::string> fx;

		size_t h = fx(s);

		auto iter = ht.find(h);
		while (iter != ht.end()) {
			if (iter->first != h) break;
			if (*iter->second == s) return iter->second;
		}

		std::string *sp = new std::string(s);
		ht.emplace(h, sp);
		return sp;
	}
}

void *ParseAlloc(void *(*mallocProc)(size_t));
void ParseFree(void *p, void (*freeProc)(void*));

void Parse(void *yyp, int yymajor, Token yyminor, Line *cookie);

void Parse(void *yyp, int yymajor, const std::string &string_value, Line *cookie)
{
	Token t;
	t.string_value = intern(string_value);
	Parse(yyp, yymajor, t, cookie);
}

void Parse(void *yyp, int yymajor, uint32_t int_value, Line *cookie)
{
	Token t;
	t.int_value = int_value;
	Parse(yyp, yymajor, t, cookie);
}


void Parse(void *yyp, int yymajor, dp_register register_value, Line *cookie)
{
	Token t;
	t.register_value = register_value;
	Parse(yyp, yymajor, t, cookie);
}


%%{
	machine lexer;

	action eol {
			line++;
			ws = 0;
			Parse(parser, tkEOL, 0, cookie);
			fgoto main;
	}

	comment := |*
		[\r|\n|\r\n] => eol;

		any {};
	*|;

	main := |*

		# white space
		[\r|\n|\r\n] => eol;

		[\t ] {
			ws++;
		};

		# comments
		'*' {
			if (!ws) fgoto comment;
			Parse(parser, tkSTAR, 0, cookie);
		};

		';' {
			fgoto comment;
		};

		# single-char ops
		'(' { Parse(parser, tkLPAREN, 0, cookie); };
		')' { Parse(parser, tkRPAREN, 0, cookie); };
		'[' { Parse(parser, tkLBRACKET, 0, cookie); };
		']' { Parse(parser, tkRBRACKET, 0, cookie); };
		#'=' { Parse(parser, tkEQ, 0, cookie); };
		'+' { Parse(parser, tkPLUS, 0, cookie); };
		'-' { Parse(parser, tkMINUS, 0, cookie); };
		'*' { Parse(parser, tkSTAR, 0, cookie); };
		'/' { Parse(parser, tkSLASH, 0, cookie); };
		'%' { Parse(parser, tkPERCENT, 0, cookie); };
		'~' { Parse(parser, tkTILDE, 0, cookie); };
		'!' { Parse(parser, tkBANG, 0, cookie); };
		'^' { Parse(parser, tkCARET, 0, cookie); };
		'&' { Parse(parser, tkAMP, 0, cookie); };
		'|' { Parse(parser, tkPIPE, 0, cookie); };
		'<' { Parse(parser, tkLT, 0, cookie); };
		'>' { Parse(parser, tkGT, 0, cookie); };
		'|' { Parse(parser, tkPIPE, 0, cookie); };
		'#' { Parse(parser, tkHASH, 0, cookie); };
		':' { Parse(parser, tkCOLON, 0, cookie); };
		#'.' { Parse(parser, tkPERIOD, 0, cookie); };
		',' { Parse(parser, tkCOMMA, 0, cookie); };

		# dp-registers -- %t0, etc
		'%' [ptv] digit+ {
			unsigned type = ts[1];
			unsigned number = scan10(ts+2, te);
			dp_register dp = {type, number };
			Parse(parser, tkDP_REGISTER, dp, cookie);
		};

		# real registers
		#'a'i { Parse(parser, tkREGISTER_A, 0, cookie); };
		'x'i { Parse(parser, tkREGISTER_X, 0, cookie); };
		'y'i { Parse(parser, tkREGISTER_Y, 0, cookie); };
		's'i { Parse(parser, tkREGISTER_S, 0, cookie); };

		# numbers
		'$' xdigit + {
			uint32_t value = scan16(ts + 1, te);
			Parse(parser, tkINTEGER, value, cookie);
		};

		'0x'i xdigit+ {
			// hexadecimal
			uint32_t value = scan16(ts + 2, te);
			Parse(parser, tkINTEGER, value, cookie);
		};

		'0b'i [01]+ {
			// binary
			uint32_t value = scan2(ts + 2, te);
			Parse(parser, tkINTEGER, value, cookie);
		};

		'%' [01]+ {
			// binary
			uint32_t value = scan2(ts + 1, te);
			Parse(parser, tkINTEGER, value, cookie);
		};

		digit+ {
			uint32_t value = scan10(ts, te);
			Parse(parser, tkINTEGER, value, cookie);
		};

		['] [^']{1,4} ['] {
			// 4 cc code

			uint32_t value = scancc(ts + 1, te - 1);
			Parse(parser, tkINTEGER, value, cookie);

		};


		# identifier
		'@' [A-Za-z0-9_]+ {
			// @ is appended to the current label.
		};


		[A-Za-z_][A-Za-z0-9_]* {
			// this may be a label or an opcode...
			// mvp or mvn generate tkOPCODE_2.
			// all others generate tkOPCODE.
			// check opcode table

			std::string s(ts, te);
			Instruction instr(m65816, s);
			if (instr) {
				// just insert the instruction directly into the cookie.
				// since lemon doesn't like c++ that much.

				cookie->instruction = instr;
				unsigned tk = tkOPCODE;
				if (instr.hasAddressMode(block)) tk = tkOPCODE_2;
				if (instr.hasAddressMode(zp_relative)) tk = tkOPCODE_2;
				Parse(parser, tk, 0, cookie);
			} else {
				Parse(parser, tkIDENTIFIER, s, cookie);
			}
		};



	*|;
}%%


%% write data;

bool parse_file(const std::string &filename)
{
	int fd;
	struct stat st;
	void *buffer;
	int ok;

	unsigned line = 1;
	unsigned ws = 0;
	Line *cookie;
	Line *prevLine;
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
	const char *eof = pe;
	const char *ts;
	const char *te;
	int cs, act;

	%% write init;
	//
	%% write exec;

	Parse(parser, 0, 0, cookie);
	ParseFree(parser, free);

	munmap(buffer, st.st_size);
	return true;

}