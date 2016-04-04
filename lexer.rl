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
#include "intern.h"
#include "cookie.h"
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
				if (c == '_') return value;
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


	unsigned error_count = 0;
	unsigned warn_count = 0;

}

void *ParseAlloc(void *(*mallocProc)(size_t));
void ParseFree(void *p, void (*freeProc)(void*));

void Parse(void *yyp, int yymajor, Token yyminor, Cookie *cookie);

void Parse(void *yyp, int yymajor, const std::string &string_value, Cookie *cookie)
{
	Token t(cookie->line_number, intern(string_value));
	Parse(yyp, yymajor, t, cookie);
}

void Parse(void *yyp, int yymajor, uint32_t value, Cookie *cookie)
{
	Token t(cookie->line_number, value);
	Parse(yyp, yymajor, t, cookie);
}


void Parse(void *yyp, int yymajor, dp_register value, Cookie *cookie)
{
	Token t(cookie->line_number, std::move(value));
	Parse(yyp, yymajor, t, cookie);
}

void Parse(void *yyp, int yymajor, ExpressionPtr value, Cookie *cookie)
{
	Token t(cookie->line_number, std::move(value));
	Parse(yyp, yymajor, t, cookie);
}


void Parse(void *yyp, int yymajor, branch::branch_type value, Cookie *cookie)
{
	Token t(cookie->line_number, value);
	Parse(yyp, yymajor, t, cookie);
}


void Parse(void *yyp, int yymajor, const Instruction &value, Cookie *cookie)
{
	Token t(cookie->line_number, std::move(value));
	Parse(yyp, yymajor, t, cookie);
}

void Parse(void *yyp, int yymajor, const std::string &string_value, branch::branch_type value, Cookie *cookie) {

	Token t(cookie->line_number, intern(string_value), value);
	Parse(yyp, yymajor, t, cookie);

}



%%{
	machine lexer;

	action eol {

			Parse(parser, tkEOL, 0, &cookie);
			Parse(parser, 0, 0, &cookie);

			line_start = te;
			cookie.line_number++;
			cookie.line_warning = false;
			cookie.line_error = false;

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

	action parse_local_identifier {
			std::string s;
			if (cookie.current_label)
				s = *cookie.current_label;

			s.assign(ts, te);
			Parse(parser, tkIDENTIFIER, s, &cookie);	
	}

	action parse_dp_register {
		unsigned type = ts[1];
		unsigned number = scan10(ts+2, te);
		dp_register dp = { type, number };
		Parse(parser, tkDP_REGISTER, dp, &cookie);
	}


	# '$' is used for statics w/in a function.
	identifier = [A-Za-z_.][A-Za-z0-9_$.]*;

	local_identifier = '@' [A-Za-z_.][A-Za-z0-9_$.]*;



	ws = [\t ]+;
	#eol = '\r\n' | \r' | '\n';

	dp_register = '%' [prtv] digit+;

	comment := |*
		'\r'|'\n'|'\r\n' => eol;

		any {};
	*|;

	error := |*

		'\r'|'\n'|'\r\n' {
			// slightly modified version of eol.
			//Parse(parser, 0, 0, &cookie); happens before we enter.
	
			line_start = te;
			cookie.line_number++;
			cookie.line_warning = false;
			cookie.line_error = false;

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
		#':' { Parse(parser, tkCOLON, 0, &cookie); };
		#'.' { Parse(parser, tkPERIOD, 0, &cookie); };
		',' { Parse(parser, tkCOMMA, 0, &cookie); };

		'*' { Parse(parser, tkSTAR, 0, &cookie); };

		# dp-registers -- %t0, etc
		dp_register => parse_dp_register;

		# real registers
		#'a'i { Parse(parser, tkREGISTER_A, std::string(ts, te), &cookie); };
		'x'i { Parse(parser, tkREGISTER_X, std::string(ts, te), &cookie); };
		'y'i { Parse(parser, tkREGISTER_Y, std::string(ts, te), &cookie); };
		'z'i { Parse(parser, tkREGISTER_Z, std::string(ts, te), &cookie); };
		's'i { Parse(parser, tkREGISTER_S, std::string(ts, te), &cookie); };

		# condition codes.
		'cc'i { Parse(parser, tkCC, std::string(ts, te), branch::cc, &cookie); };
		'cs'i { Parse(parser, tkCC, std::string(ts, te), branch::cs, &cookie); };

		'eq'i { Parse(parser, tkCC, std::string(ts, te), branch::eq, &cookie); };
		'ne'i { Parse(parser, tkCC, std::string(ts, te), branch::ne, &cookie); };

		'mi'i { Parse(parser, tkCC, std::string(ts, te), branch::mi, &cookie); };
		'pl'i { Parse(parser, tkCC, std::string(ts, te), branch::pl, &cookie); };

		'vc'i { Parse(parser, tkCC, std::string(ts, te), branch::vc, &cookie); };
		'vs'i { Parse(parser, tkCC, std::string(ts, te), branch::vs, &cookie); };

		'gt'i { Parse(parser, tkCC, std::string(ts, te), branch::unsigned_gt, &cookie); };
		'ge'i { Parse(parser, tkCC, std::string(ts, te), branch::unsigned_ge, &cookie); };

		'lt'i { Parse(parser, tkCC, std::string(ts, te), branch::unsigned_lt, &cookie); };
		'le'i { Parse(parser, tkCC, std::string(ts, te), branch::unsigned_le, &cookie); };

		'signed'i { Parse(parser, tkSIGNED, std::string(ts, te), &cookie); };
		'unsigned'i { Parse(parser, tkUNSIGNED, std::string(ts, te), &cookie); };


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

		'0b'i [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'%' [01][01_]* {
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

		'%weak' {
			std::string s(ts, te);
			Parse(parser, tkWEAK, s, &cookie);
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
				else Parse(parser, tkEXPRESSION, e, &cookie);
			} else {
				Parse(parser, tkIDENTIFIER, s, &cookie);
			}

		};

		local_identifier => parse_local_identifier;

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
		#':' { Parse(parser, tkCOLON, 0, &cookie); };
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

		'0b'i [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'%' [01][01_]* {
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

		'%weak' {
			std::string s(ts, te);
			Parse(parser, tkWEAK, s, &cookie);
		};

		identifier => parse_identifier;
		local_identifier => parse_local_identifier;

		any => error;

	*|;


	operand_reg := |*

		'\r'|'\n'|'\r\n' => eol;
		ws {};
		';' { fgoto comment; };

		dp_register => parse_dp_register;

		#'a'i { Parse(parser, tkREGISTER_A, 0, &cookie); };
		'x'i { Parse(parser, tkREGISTER_X, 0, &cookie); };
		'y'i { Parse(parser, tkREGISTER_Y, 0, &cookie); };
		'z'i { Parse(parser, tkREGISTER_Z, 0, &cookie); };
		's'i { Parse(parser, tkREGISTER_S, 0, &cookie); };

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

		'volatile'i {
			Parse(parser, tkVOLATILE, 0, &cookie);
		};

		'noreturn'i {
			Parse(parser, tkNORETURN, 0, &cookie);
		};

		'dynamic'i {
			Parse(parser, tkDYNAMIC, 0, &cookie);
		};

		'databank'i {
			Parse(parser, tkDATABANK, 0, &cookie);
		};

		'debug'i {
			Parse(parser, tkDEBUG, 0, &cookie);
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
		'naked'i {
			Parse(parser, tkNAKED, 0, &cookie);
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

		'0b'i [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 2, te);
			Parse(parser, tkINTEGER, value, &cookie);
		};

		'%' [01][01_]* {
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

		'align'i {
			Parse(parser, tkALIGN, 0, &cookie);
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

		'import'i {
			Parse(parser, tkIMPORT, 0, &cookie);
			next_operand = lexer_en_operand_no_reg;
		};

		'strong'i {
			Parse(parser, tkSTRONG, 0, &cookie);
			next_operand = lexer_en_operand_no_reg;
		};


#		'begin_stack'i {
#			Parse(parser, tkFX_PROLOGUE, 0, &cookie);
#		};

#		'end_stack'i {
#			Parse(parser, tkFX_EPILOGUE, 0, &cookie);
#		};

		# smart branches.

		'branch'i {
			Parse(parser, tkBRANCH, 0, &cookie);
		};

		'__bra'i {
			Parse(parser, tkSMART_BRANCH, branch::always, &cookie);
		};
		'__beq'i {
			Parse(parser, tkSMART_BRANCH, branch::eq, &cookie);
		};
		'__bne'i {
			Parse(parser, tkSMART_BRANCH, branch::ne, &cookie);
		};

		'__bcc'i {
			Parse(parser, tkSMART_BRANCH, branch::cc, &cookie);
		};
		'__bcs'i {
			Parse(parser, tkSMART_BRANCH, branch::cs, &cookie);
		};

		'__bvc'i {
			Parse(parser, tkSMART_BRANCH, branch::vc, &cookie);
		};
		'__bvs'i {
			Parse(parser, tkSMART_BRANCH, branch::vs, &cookie);
		};

		'__bmi'i {
			Parse(parser, tkSMART_BRANCH, branch::mi, &cookie);
		};
		'__bpl'i {
			Parse(parser, tkSMART_BRANCH, branch::pl, &cookie);
		};


		'__bugt'i {
			Parse(parser, tkSMART_BRANCH, branch::unsigned_gt, &cookie);
		};
		'__buge'i {
			Parse(parser, tkSMART_BRANCH, branch::unsigned_ge, &cookie);
		};
		'__bult'i {
			Parse(parser, tkSMART_BRANCH, branch::unsigned_lt, &cookie);
		};
		'__bule'i {
			Parse(parser, tkSMART_BRANCH, branch::unsigned_le, &cookie);
		};
		'__bsgt'i {
			Parse(parser, tkSMART_BRANCH, branch::signed_gt, &cookie);
		};
		'__bsge'i {
			Parse(parser, tkSMART_BRANCH, branch::signed_ge, &cookie);
		};
		'__bslt'i {
			Parse(parser, tkSMART_BRANCH, branch::signed_lt, &cookie);
		};
		'__bsle'i {
			Parse(parser, tkSMART_BRANCH, branch::signed_le, &cookie);
		};


		[A-Za-z_][A-Za-z0-9_]* {

			// mvp or mvn generate tkOPCODE_2.
			// all others generate tkOPCODE.
			// check opcode table

			std::string s(ts, te);
			Instruction instr(m65816, s);
			if (instr) {
				// just insert the instruction directly into the cookie.
				// since it's easier than dealing with c++ unions.

				//cookie.scratch.instruction = instr;
				unsigned tk = tkOPCODE;
				if (instr.hasAddressMode(block)) tk = tkOPCODE_2;
				if (instr.hasAddressMode(zp_relative)) tk = tkOPCODE_2;
				Parse(parser, tk, instr, &cookie);
				//fgoto operand;

			} else {
				error("Invalid opcode/directive", cookie);
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

		local_identifier => parse_local_identifier;

		# this is here so we can have label:
		# maybe we should just drop the :
		#':' { Parse(parser, tkCOLON, 0, &cookie); };

		any {
			fprintf(stderr, "Unable to lex!\n");
			// clear out the parser
			Parse(parser, 0, 0, &cookie);
			fgoto error;
		};

	*|;

}%%


%% write data;
%% access cookie.fsm.;



void error(const std::string &s, Cookie &cookie) {

	if (!cookie.line_error && !cookie.line_warning) {
		const char *p = cookie.fsm.line_start;
		while (p != cookie.fsm.line_end) {
			char c = *p++;
			if (c == '\r' || c == '\n') break;
			fputc(c, stderr);
		}
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "Error: Line %u: %s\n", cookie.line_number, s.c_str());

	cookie.line_error = true;
	error_count++;
}

void warn(const std::string &s, Cookie &cookie) {

	if (!cookie.line_error && !cookie.line_warning) {
		const char *p = cookie.fsm.line_start;
		while (p != cookie.fsm.line_end) {
			char c = *p++;
			if (c == '\r' || c == '\n') break;
			fputc(c, stderr);
		}
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "Error: Line %u: %s\n", cookie.line_number, s.c_str());

	cookie.line_warning = true;
	warn_count++;
}

void parse_text(void *parser, Cookie &cookie, const char *p, const char *pe, bool end_of_file) {
	
	const char *eof = end_of_file ? pe : nullptr;
	//int cs, act;

	auto &te = cookie.fsm.te;
	auto &ts = cookie.fsm.ts;
	auto &next_operand = cookie.fsm.next_operand;
	auto &line_start = cookie.fsm.line_start;
	auto &line_end = cookie.fsm.line_end;

	line_start = p;
	line_end = pe;

	%% write exec;


	// should return error state?

	if (end_of_file) {

		Parse(parser, tkEOL, 0, &cookie);
		Parse(parser, 0, 0, &cookie);
	}
}


void ParseTrace(FILE *TraceFILE, char *zTracePrompt);

void *init(Cookie &cookie) {
	
	void *parser;

	parser = ParseAlloc(malloc);

	//ParseTrace(stderr, (char *)": ");

	cookie.data_segment.reset(new Segment);
	cookie.data_segment->convention = Segment::data;


	%% write init;

	return parser;
}


void cleanup_cookie(Cookie &cookie, SegmentQueue &rv) {

	cookie.segments.emplace_back(std::move(cookie.data_segment));

	// set exports.
	for (auto &seg : cookie.segments) {

		identifier label = seg->name;
		if (label && cookie.export_set.count(label))
			seg->global = true;

		for (auto line : seg->lines) {
			identifier label = line->label;
			if (label && cookie.export_set.count(label))
				line->global = true;
		}
	}

	rv = std::move(cookie.segments);
}


bool parse_file(FILE *file, SegmentQueue &rv) {



	Cookie cookie;

	void *parser = init(cookie);

	for(;;) {
		char *cp;
		size_t len;

		cp = fgetln(file, &len);

		if (!cp) break;

		// 
		parse_text(parser, cookie, cp, cp + len, false);

	}
	const char *eofstr="";
	parse_text(parser, cookie, eofstr, eofstr, true);


	ParseFree(parser, free);

	if (ferror(file)) {

		fprintf(stderr, "Error reading from file\n");
		return false;
	}

	if (error_count > 0) {
		return false;
	}

	cleanup_cookie(cookie, rv);
	return true;
}


bool parse_file(const std::string &filename, SegmentQueue &rv)
{
	int fd;
	struct stat st;
	void *buffer;
	int ok;

	Cookie cookie;

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

	parser = init(cookie);


	const char *p = (const char *)buffer;
	const char *pe = (const char *)buffer + st.st_size;
	const char *eof = pe;


	parse_text(parser, cookie, p, pe, true);

	ParseFree(parser, free);

	munmap(buffer, st.st_size);

	if (error_count > 0) {
		// todo -- kill data, if needed.
		return false;
	}

	cleanup_cookie(cookie, rv);
	return true;
}