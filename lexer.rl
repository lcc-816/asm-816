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


}


void Parser::parse(int yymajor, const std::string &string_value)
{
	Token t(line_number, intern(string_value));
	parse(yymajor, std::move(t));
}

void Parser::parse(int yymajor, uint32_t value)
{
	Token t(line_number, value);
	parse(yymajor, std::move(t));
}


void Parser::parse(int yymajor, dp_register value)
{
	Token t(line_number, std::move(value));
	parse(yymajor, std::move(t));
}

void Parser::parse(int yymajor, ExpressionPtr value)
{
	Token t(line_number, std::move(value));
	parse(yymajor, std::move(t));
}


void Parser::parse(int yymajor, branch::branch_type value)
{
	Token t(line_number, value);
	parse(yymajor, std::move(t));
}


void Parser::parse(int yymajor, const Instruction &value)
{
	Token t(line_number, std::move(value));
	parse(yymajor, std::move(t));
}

void Parser::parse(int yymajor, const std::string &string_value, branch::branch_type value) {

	Token t(line_number, intern(string_value), value);
	parse(yymajor, std::move(t));

}



%%{
	machine lexer;

	action eol {

			parse(tkEOL, 0);
			parse(0, 0);

			line_start = te;
			line_number++;
			line_warning = false;
			line_error = false;

			next_operand = 0;
			fgoto main;
	}

	action error {
			fprintf(stderr, "Unable to lex!\n");
			// clear out the parser
			parse(0, 0);
			fgoto error;
	}

	action parse_identifier {
			std::string s(ts, te);
			parse(tkIDENTIFIER, s);	
	}

	action parse_local_identifier {
			std::string s;
			if (current_label)
				s = *current_label;

			s.assign(ts, te);
			parse(tkIDENTIFIER, s);	
	}

	action parse_dp_register {
		unsigned type = ts[1];
		unsigned number = scan10(ts+2, te);
		dp_register dp = { type, number };
		parse(tkDP_REGISTER, dp);
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
			//parse(0, 0); happens before we enter.
	
			line_start = te;
			line_number++;
			line_warning = false;
			line_error = false;

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
		'(' { parse(tkLPAREN, 0); };
		')' { parse(tkRPAREN, 0); };
		'[' { parse(tkLBRACKET, 0); };
		']' { parse(tkRBRACKET, 0); };
		'=' { parse(tkEQ, 0); };
		'+' { parse(tkPLUS, 0); };
		'-' { parse(tkMINUS, 0); };
		'*' { parse(tkSTAR, 0); };
		'/' { parse(tkSLASH, 0); };
		'%' { parse(tkPERCENT, 0); };
		'~' { parse(tkTILDE, 0); };
		'!' { parse(tkBANG, 0); };
		'^' { parse(tkCARET, 0); };
		'&' { parse(tkAMP, 0); };
		'|' { parse(tkPIPE, 0); };
		'<' { parse(tkLT, 0); };
		'>' { parse(tkGT, 0); };
		'|' { parse(tkPIPE, 0); };
		'#' { parse(tkHASH, 0); };
		#':' { parse(tkCOLON, 0); };
		#'.' { parse(tkPERIOD, 0); };
		',' { parse(tkCOMMA, 0); };

		'*' { parse(tkSTAR, 0); };

		# dp-registers -- %t0, etc
		dp_register => parse_dp_register;

		# real registers
		#'a'i { parse(tkREGISTER_A, std::string(ts, te)); };
		'x'i { parse(tkREGISTER_X, std::string(ts, te)); };
		'y'i { parse(tkREGISTER_Y, std::string(ts, te)); };
		'z'i { parse(tkREGISTER_Z, std::string(ts, te)); };
		's'i { parse(tkREGISTER_S, std::string(ts, te)); };

		# condition codes.
		'cc'i { parse(tkCC, std::string(ts, te), branch::cc); };
		'cs'i { parse(tkCC, std::string(ts, te), branch::cs); };

		'eq'i { parse(tkCC, std::string(ts, te), branch::eq); };
		'ne'i { parse(tkCC, std::string(ts, te), branch::ne); };

		'mi'i { parse(tkCC, std::string(ts, te), branch::mi); };
		'pl'i { parse(tkCC, std::string(ts, te), branch::pl); };

		'vc'i { parse(tkCC, std::string(ts, te), branch::vc); };
		'vs'i { parse(tkCC, std::string(ts, te), branch::vs); };

		'gt'i { parse(tkCC, std::string(ts, te), branch::unsigned_gt); };
		'ge'i { parse(tkCC, std::string(ts, te), branch::unsigned_ge); };

		'lt'i { parse(tkCC, std::string(ts, te), branch::unsigned_lt); };
		'le'i { parse(tkCC, std::string(ts, te), branch::unsigned_le); };

		'signed'i { parse(tkSIGNED, std::string(ts, te)); };
		'unsigned'i { parse(tkUNSIGNED, std::string(ts, te)); };


		# numbers
		'$' xdigit + {
			uint32_t value = scan16(ts + 1, te);
			parse(tkINTEGER, value);
		};

		'0x'i xdigit+ {
			// hexadecimal
			uint32_t value = scan16(ts + 2, te);
			parse(tkINTEGER, value);
		};

		'0b'i [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 2, te);
			parse(tkINTEGER, value);
		};

		'%' [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 1, te);
			parse(tkINTEGER, value);
		};

		digit+ {
			uint32_t value = scan10(ts, te);
			parse(tkINTEGER, value);
		};

		['] [^']{1,4} ['] {
			// 4 cc code

			uint32_t value = scancc(ts + 1, te - 1);
			parse(tkINTEGER, value);

		};

		# strings.
		["] ([^"])* ["] {
			std::string s(ts + 1, te - 1);
			parse(tkSTRING, s);
		};

		'%weak' {
			std::string s(ts, te);
			parse(tkWEAK, s);
		};

		#identifier => parse_identifier;

		identifier => {
			// may be an equate...
			std::string s(ts, te);
			auto id = intern(s);

			ExpressionPtr e = equates.find(id);
			if (e) {
				dp_register dp;
				// register is a special case...

				if (e->is_register(dp)) {
					parse(tkDP_REGISTER, dp);
				}
				else parse(tkEXPRESSION, e);
			} else {
				parse(tkIDENTIFIER, s);
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
		'(' { parse(tkLPAREN, 0); };
		')' { parse(tkRPAREN, 0); };
		'[' { parse(tkLBRACKET, 0); };
		']' { parse(tkRBRACKET, 0); };
		'=' { parse(tkEQ, 0); };
		'+' { parse(tkPLUS, 0); };
		'-' { parse(tkMINUS, 0); };
		'*' { parse(tkSTAR, 0); };
		'/' { parse(tkSLASH, 0); };
		'%' { parse(tkPERCENT, 0); };
		'~' { parse(tkTILDE, 0); };
		'!' { parse(tkBANG, 0); };
		'^' { parse(tkCARET, 0); };
		'&' { parse(tkAMP, 0); };
		'|' { parse(tkPIPE, 0); };
		'<' { parse(tkLT, 0); };
		'>' { parse(tkGT, 0); };
		'|' { parse(tkPIPE, 0); };
		'#' { parse(tkHASH, 0); };
		#':' { parse(tkCOLON, 0); };
		#'.' { parse(tkPERIOD, 0); };
		',' { parse(tkCOMMA, 0); };

		'*' { parse(tkSTAR, 0); };

		'<<' { parse(tkLTLT, 0); };
		'>>' { parse(tkGTGT, 0); };

		# numbers
		'$' xdigit + {
			uint32_t value = scan16(ts + 1, te);
			parse(tkINTEGER, value);
		};

		'0x'i xdigit+ {
			// hexadecimal
			uint32_t value = scan16(ts + 2, te);
			parse(tkINTEGER, value);
		};

		'0b'i [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 2, te);
			parse(tkINTEGER, value);
		};

		'%' [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 1, te);
			parse(tkINTEGER, value);
		};

		digit+ {
			uint32_t value = scan10(ts, te);
			parse(tkINTEGER, value);
		};

		['] [^']{1,4} ['] {
			// 4 cc code

			uint32_t value = scancc(ts + 1, te - 1);
			parse(tkINTEGER, value);

		};

		# strings.
		["] ([^"])* ["] {
			std::string s(ts, te);
			parse(tkSTRING, s);
		};

		'%weak' {
			std::string s(ts, te);
			parse(tkWEAK, s);
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

		#'a'i { parse(tkREGISTER_A, 0); };
		'x'i { parse(tkREGISTER_X, 0); };
		'y'i { parse(tkREGISTER_Y, 0); };
		'z'i { parse(tkREGISTER_Z, 0); };
		's'i { parse(tkREGISTER_S, 0); };

		any => error;
	*|;

	operand_pragma := |*

		# white space
		'\r'|'\n'|'\r\n' => eol;

		[\t ]+ {};

		';' { fgoto comment; };

		'=' { parse(tkEQ, 0); };
		',' { parse(tkCOMMA, 0); };

		'segment'i {
			parse(tkSEGMENT, 0);
		};

		'volatile'i {
			parse(tkVOLATILE, 0);
		};

		'noreturn'i {
			parse(tkNORETURN, 0);
		};

		'dynamic'i {
			parse(tkDYNAMIC, 0);
		};

		'databank'i {
			parse(tkDATABANK, 0);
		};

		'debug'i {
			parse(tkDEBUG, 0);
		};

		'private'i {
			parse(tkPRIVATE, 0);
		};

		'kind'i {
			parse(tkKIND, 0);
		};

		'cdecl'i {
			parse(tkCDECL, 0);
		};
		'pascal'i {
			parse(tkPASCAL, 0);
		};
		'stdcall'i {
			parse(tkSTDCALL, 0);
		};
		'naked'i {
			parse(tkNAKED, 0);
		};


		'void'i {
			parse(tkVOID, 0);
		};

		'rts'i {
			parse(tkRTS, 0);
		};

		'locals'i {
			parse(tkLOCALS, 0);
		};

		'parameters'i {
			parse(tkPARAMETERS, 0);
		};

		'return'i {
			parse(tkRETURN, 0);
		};

		# numbers
		'$' xdigit + {
			uint32_t value = scan16(ts + 1, te);
			parse(tkINTEGER, value);
		};

		'0x'i xdigit+ {
			// hexadecimal
			uint32_t value = scan16(ts + 2, te);
			parse(tkINTEGER, value);
		};

		'0b'i [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 2, te);
			parse(tkINTEGER, value);
		};

		'%' [01][01_]* {
			// binary
			uint32_t value = scan2(ts + 1, te);
			parse(tkINTEGER, value);
		};

		digit+ {
			uint32_t value = scan10(ts, te);
			parse(tkINTEGER, value);
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
			parse(tkDCB, 0);
			next_operand = lexer_en_operand_no_reg;
		};

		'dc.w'i {
			parse(tkDCW, 0);
			next_operand = lexer_en_operand_no_reg;
		};

		'dc.l'i {
			parse(tkDCL, 0);
			next_operand = lexer_en_operand_no_reg;
		};

		'ds'i {
			parse(tkDS, 0);
			next_operand = lexer_en_operand_no_reg;
		};

		'align'i {
			parse(tkALIGN, 0);
			next_operand = lexer_en_operand_no_reg;
		};

		'pragma'i {
			parse(tkPRAGMA, 0);
			next_operand = lexer_en_operand_pragma;
		};

		'equ'i {
			parse(tkEQU, 0);
		};

		# create a separate context for first token vs
		# checking whitespace?
		'start'i {
			parse(tkSTART, 0);
		};

		'data'i {
			parse(tkDATA, 0);
		};

		'end'i {
			parse(tkEND, 0);
		};

		'export'i {
			parse(tkEXPORT, 0);
			next_operand = lexer_en_operand_no_reg;
		};

		'import'i {
			parse(tkIMPORT, 0);
			next_operand = lexer_en_operand_no_reg;
		};

		'strong'i {
			parse(tkSTRONG, 0);
			next_operand = lexer_en_operand_no_reg;
		};


#		'begin_stack'i {
#			parse(tkFX_PROLOGUE, 0);
#		};

#		'end_stack'i {
#			parse(tkFX_EPILOGUE, 0);
#		};

		# smart branches.

		'branch'i {
			parse(tkBRANCH, 0);
		};

		'__bra'i {
			parse(tkSMART_BRANCH, branch::always);
		};
		'__beq'i {
			parse(tkSMART_BRANCH, branch::eq);
		};
		'__bne'i {
			parse(tkSMART_BRANCH, branch::ne);
		};

		'__bcc'i {
			parse(tkSMART_BRANCH, branch::cc);
		};
		'__bcs'i {
			parse(tkSMART_BRANCH, branch::cs);
		};

		'__bvc'i {
			parse(tkSMART_BRANCH, branch::vc);
		};
		'__bvs'i {
			parse(tkSMART_BRANCH, branch::vs);
		};

		'__bmi'i {
			parse(tkSMART_BRANCH, branch::mi);
		};
		'__bpl'i {
			parse(tkSMART_BRANCH, branch::pl);
		};


		'__bugt'i {
			parse(tkSMART_BRANCH, branch::unsigned_gt);
		};
		'__buge'i {
			parse(tkSMART_BRANCH, branch::unsigned_ge);
		};
		'__bult'i {
			parse(tkSMART_BRANCH, branch::unsigned_lt);
		};
		'__bule'i {
			parse(tkSMART_BRANCH, branch::unsigned_le);
		};
		'__bsgt'i {
			parse(tkSMART_BRANCH, branch::signed_gt);
		};
		'__bsge'i {
			parse(tkSMART_BRANCH, branch::signed_ge);
		};
		'__bslt'i {
			parse(tkSMART_BRANCH, branch::signed_lt);
		};
		'__bsle'i {
			parse(tkSMART_BRANCH, branch::signed_le);
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
				parse(tk, instr);
				//fgoto operand;

			} else {
				error("Invalid opcode/directive");
				parse(0, 0);
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
		#':' { parse(tkCOLON, 0); };

		any {
			fprintf(stderr, "Unable to lex!\n");
			// clear out the parser
			parse(0, 0);
			fgoto error;
		};

	*|;

}%%


%% write data;
%% access fsm.;



void Parser::error(const std::string &s) {

	if (!line_error && !line_warning) {
		const char *p = fsm.line_start;
		while (p != fsm.line_end) {
			char c = *p++;
			if (c == '\r' || c == '\n') break;
			fputc(c, stderr);
		}
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "Error: Line %u: %s\n", line_number, s.c_str());

	line_error = true;
	error_count++;
}

void Parser::warn(const std::string &s) {

	if (!line_error && !line_warning) {
		const char *p = fsm.line_start;
		while (p != fsm.line_end) {
			char c = *p++;
			if (c == '\r' || c == '\n') break;
			fputc(c, stderr);
		}
		fprintf(stderr, "\n");
	}

	fprintf(stderr, "Error: Line %u: %s\n", line_number, s.c_str());

	line_warning = true;
	warn_count++;
}

void Parser::parse_text(const char *p, const char *pe, bool end_of_file) {
	
	const char *eof = end_of_file ? pe : nullptr;
	//int cs, act;

	auto &te = fsm.te;
	auto &ts = fsm.ts;
	auto &next_operand = fsm.next_operand;
	auto &line_start = fsm.line_start;
	auto &line_end = fsm.line_end;

	line_start = p;
	line_end = pe;

	%% write exec;


	// should return error state?

	if (end_of_file) {

		parse(tkEOL, 0);
		parse(0, 0);
	}
}


Parser::Parser() {
	
	data_segment.reset(new Segment);
	data_segment->convention = Segment::data;

	%% write init;
}


void Parser::get_segments(SegmentQueue &rv) {

	segments.emplace_back(std::move(data_segment));

	// set exports.
	for (auto &seg : segments) {

		identifier label = seg->name;
		if (label && export_set.count(label))
			seg->global = true;

		for (auto line : seg->lines) {
			identifier label = line->label;
			if (label && export_set.count(label))
				line->global = true;
		}
	}

	rv = std::move(segments);
}


bool parse_file(FILE *file, SegmentQueue &rv) {


	auto parser = Parser::make();

	for(;;) {
		char *cp;
		size_t len;

		cp = fgetln(file, &len);

		if (!cp) break;

		// 
		parser->parse_text(cp, cp + len, false);

	}
	const char *eofstr="";
	parser->parse_text(eofstr, eofstr, true);


	if (ferror(file)) {

		fprintf(stderr, "Error reading from file\n");
		return false;
	}

	if (parser->error_count > 0) {
		return false;
	}

	parser->get_segments(rv);
	return true;
}


bool parse_file(const std::string &filename, SegmentQueue &rv)
{
	int fd;
	struct stat st;
	void *buffer;
	int ok;


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

	auto parser = Parser::make();

	const char *p = (const char *)buffer;
	const char *pe = (const char *)buffer + st.st_size;
	const char *eof = pe;


	parser->parse_text(p, pe, true);

	munmap(buffer, st.st_size);

	if (parser->error_count > 0) {
		// todo -- kill data, if needed.
		return false;
	}

	parser->get_segments(rv);
	return true;
}