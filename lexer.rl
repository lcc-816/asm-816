#include <string>
#include <algorithm>
#include <cctype>

namespace {

	struct Cookie {
		// Line data..
		std::string *label;

		Mnemonic mnemonic;
		AddressMode mode;
		unsigned instruction_size;
		unsigned mx; // mx bits...
		expression_ptr operands[2];

		unsigned directive;
		// ...
	};

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
			Parse(parse, tkSTAR, 0, cookie);
		};

		';' {
			fgoto comment;
		};

		# single-char ops
		'(' { Parse(parser, tkLPAREN, 0, cookie); };
		')' { Parse(parser, tkRPAREN, 0, cookie); };
		'[' { Parse(parser, tkLBRACKET, 0, cookie); };
		']' { Parse(parser, tkRBRACKET, 0, cookie); };
		'=' { Parse(parser, tkEQ, 0, cookie); };
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
		'.' { Parse(parser, tkPERIOD, 0, cookie); };
		',' { Parse(parser, tkCOMMA, 0, cookie); };

		# dp-registers -- %t0, etc
		'%' [ptv] digit+ {
			unsigned type = ts[1];
			unsigned number = scan10(ts+2, te);
			Parse(parser, tkDP_REGISTER, 0, cookie);
		};

		# real registers
		'a'i { Parse(parser, tkREGISTER_A, 0, cookie); };
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
		};



	*|;
}%%


%% write data;

bool parse_file(const std::string &filename)
{
	

	//
	%% write init;
	//

	%% write exec;

}