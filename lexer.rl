#include <string>
#include <algorithm>
#include <numeric>
#include <vector>
#include <cstdint>


#include "lexer.h"
#include "grammar.h"
#include "intern.h"
#include "pragma.h"

%%{
	machine lexer;

	action eol {
		parse(tkEOL);
		_line++;

		fgoto main;
	}


	action error_eol {
		parse(tkEOL);
		_line++;

		fgoto main;
	}


	action error {
		fprintf(stderr, "#%s:%d unexpected character '%c'\n", _file ? _file->c_str() : "-", _line, fc);
		_error++;
		fgoto error;
	}

	action parse_identifier {
		std::string s(ts, te);
		parse(tkIDENTIFIER, std::move(s));	
	}


	action parse_local_identifier {
		std::string s(ts, te);
		parse(tkAT_IDENTIFIER, std::move(s));
	}


	action parse_dp_register {
		unsigned type = ts[1];
		unsigned number = scan10(ts+2, te);
		dp_register dp = { type, number };
		parse(tkDP_REGISTER, dp);
	}


	# '$' is used for statics w/in a function.
	identifier = [A-Za-z_][A-Za-z0-9_$]*;

	local_identifier = '@' identifier;



	ws = [\t ]+;
	eol = '\r' | '\n' | '\r\n';

	dp_register = '%' [prtv] digit+;

	comment := |*
		'\r'|'\n'|'\r\n' => eol;

		any;
	*|;

	error := |*

		eol => error_eol;

		any;
	*|;



	opcode := |*

		# white space
		' ' ;
		'\t';

		eol => eol;


		';' { fgoto comment; };

		# single-char ops
		'{' { parse(tkLBRACKET); };
		'}' { parse(tkRBRACKET); };
		'(' { parse(tkLPAREN); };
		')' { parse(tkRPAREN); };
		'[' { parse(tkLBRACKET); };
		']' { parse(tkRBRACKET); };
		'=' { parse(tkEQ); };
		'+' { parse(tkPLUS); };
		'-' { parse(tkMINUS); };
		'*' { parse(tkSTAR); };
		'/' { parse(tkSLASH); };
		'%' { parse(tkPERCENT); };
		'~' { parse(tkTILDE); };
		'!' { parse(tkBANG); };
		'^' { parse(tkCARET); };
		'&' { parse(tkAMP); };
		'|' { parse(tkPIPE); };
		'<' { parse(tkLT); };
		'>' { parse(tkGT); };
		'|' { parse(tkPIPE); };
		'#' { parse(tkHASH); };
		':' { parse(tkCOLON); };
		'.' { parse(tkDOT); };
		',' { parse(tkCOMMA); };

		'*' { parse(tkSTAR); };


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

		'0q'i [0123][0123_]* {
			// quaternary
			uint32_t value = scan4(ts + 2, te);
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
			parse(tkSTRING, std::move(s));
		};



		# dp-registers -- %t0, etc
		dp_register => parse_dp_register;

		# real registers
		'a'i { parse(tkREGISTER_A, std::string(ts, te)); };
		'b'i { parse(tkREGISTER_B, std::string(ts, te)); };
		'd'i { parse(tkREGISTER_D, std::string(ts, te)); };
		'k'i { parse(tkREGISTER_K, std::string(ts, te)); };
		'p'i { parse(tkREGISTER_P, std::string(ts, te)); };
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

		'signed'i   { parse(tkSIGNED, std::string(ts, te)); };
		'unsigned'i { parse(tkUNSIGNED, std::string(ts, te)); };

		'if'i      { parse(tkIF, std::string(ts, te)); };
		'do'i      { parse(tkDO, std::string(ts, te)); };
		'while'i   { parse(tkWHILE, std::string(ts, te)); };
		'push'i    { parse(tkPUSH, std::string(ts, te)); };
		'pull'i    { parse(tkPULL, std::string(ts, te)); };

		'branch'i  { parse(tkBRANCH, std::string(ts, te)); };


		'%cdecl'   { parse(tkCDECL, std::string(ts, te)); };
		'%pascal'  { parse(tkPASCAL, std::string(ts, te)); };
		'%stdcall' { parse(tkSTDCALL, std::string(ts, te)); };
		'%weak'    { parse(tkWEAK, std::string(ts, te)); };
#		'%void'    { parse(tkVOID, std::string(ts, te)); };
		'%sizeof'  { parse(tkSIZEOF, std::string(ts, te)); };


		'dc'i      { parse(tkDC, std::string(ts, te)); };
		'dcb'i     { parse(tkDCB, std::string(ts, te)); };
		'ds'i      { parse(tkDS, std::string(ts, te)); };
		'align'i   { parse(tkALIGN, std::string(ts, te)); };
		'pragma'i  { parse(tkPRAGMA, std::string(ts, te)); };
		'equ'i     { parse(tkEQU, std::string(ts, te)); };
		'export'i  { parse(tkEXPORT, std::string(ts, te)); };
		'import'i  { parse(tkIMPORT, std::string(ts, te)); };
		'strong'i  { parse(tkSTRONG, std::string(ts, te)); };
		'include'i { parse(tkINCLUDE, std::string(ts, te)); };


		# handle opcodes / macros later!

		identifier           { parse(tkIDENTIFIER, std::string(ts, te), 0); };
		'@' identifier       { parse(tkIDENTIFIER, std::string(ts, te), 1); };

		'&...'               { parse(tkMPARAM, std::string(ts,te), 2); };
		'&' identifier       { parse(tkMPARAM, std::string(ts,te), 0); };
		'&' identifier '...' { parse(tkMPARAM, std::string(ts,te), 1); };


		any => error;
	*|;




	main := |*
		# white space
		eol => eol;

		# comments
		[*;#!] { fgoto comment; };

		'function'i {
			parse(tkFUNCTION, std::string(ts, te));
			fgoto opcode;
		};

		'data'i {
			parse(tkDATA, std::string(ts, te));
			fgoto opcode;
		};

		'record'i {
			parse(tkRECORD, std::string(ts, te));
			fgoto opcode;
		};

		'macro'i {
			parse(tkMACRO, std::string(ts, te));
			fgoto opcode;
		};

		identifier           { parse(tkLABEL, std::string(ts, te), 0); };
		'@' identifier       { parse(tkLABEL, std::string(ts, te), 1); };
		'&' identifier       { parse(tkLABEL, std::string(ts, te), 2); };
		'&' identifier '...' { parse(tkLABEL, std::string(ts, te), 2); };

		# todo -- & identifier is valid in a macro and should convert to a label...
		# store the column in the token?
		any => {
			fhold;
			fgoto opcode;
		};

	*|;


}%%



namespace {


	int tox(char c)
	{
		c |= 0x20; // lowercase it.
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		return 0;
	}

	uint32_t scan10(const uint8_t *begin, const uint8_t *end)
	{
		return std::accumulate(begin, end, 0, 
			[](uint32_t value, char c){
				return value * 10 + c - '0';
			});
	}

	uint32_t scan16(const uint8_t *begin, const uint8_t *end)
	{
		return std::accumulate(begin, end, 0, 
			[](uint32_t value, char c){
				return (value << 4) + tox(c);
			});
	}

	uint32_t scan2(const uint8_t *begin, const uint8_t *end)
	{
		return std::accumulate(begin, end, 0, 
			[](uint32_t value, char c){
				if (c == '_') return value;
				return (value << 1) + (c & 0x01);
			});
	}

	uint32_t scan4(const uint8_t *begin, const uint8_t *end)
	{
		return std::accumulate(begin, end, 0, 
			[](uint32_t value, char c){
				if (c == '_') return value;
				return (value << 2) + (c & 0x03);
			});
	}


	uint32_t scancc(const uint8_t *begin, const uint8_t *end)
	{
		return std::accumulate(begin, end, 0,
			[](uint32_t value, char c)
			{
				return (value << 8) + (unsigned)c;
			}
		);

	}

	%% write data;
}

#define parse(...) _parse(fx, __VA_ARGS__)

lexer::lexer(const std::string &s) : lexer(intern(s)) {}
lexer::lexer(std::string &&s) : lexer(intern(std::move(s))) {}

lexer::lexer(identifier file) : _file(file) {
	char *ts;
	char *te;

	%%write init;
}



std::vector<Token> lexer::tokenize(const void *begin, const void *end, bool is_eof) {
	token_vector rv;

	tokenize(begin, end, is_eof, [&rv](Token &&t){ rv.emplace_back(std::move(t)); });

	return rv;
}


void lexer::tokenize(const void *begin, const void *end, bool is_eof, token_function fx) {

	if (!fx) fx = [](Token &&){};

	const uint8_t *p, *pe, *eof;
	const uint8_t *ts, *te;


	auto offset = buffer.size();

	if (offset == 0) {
		p = (const uint8_t *)begin;
		pe = (const uint8_t *)end;
		ts = te = nullptr;
	} else {
		buffer.insert(buffer.end(), (const uint8_t *)begin, (const uint8_t *)end);
		p = buffer.data() + offset;
		pe = buffer.data() + buffer.size();

		ts = buffer.data();
		te = buffer.data() + offset;
	}

	eof = is_eof ? pe : nullptr;

	%%write exec;

	// todo -- store from start of line?
	if (ts == 0) {
		buffer.clear();
	} else {
		// buffer may or may not have data in it already...
		if (buffer.empty()) {
			buffer.insert(buffer.end(), ts, pe);
		} else {
			offset = ts - buffer.data();
			buffer.erase(buffer.begin(), buffer.begin() + offset);
		}

	}

	if (eof && _last && _last != tkEOL) {
		_parse(fx, tkEOL);
	}
}

void lexer::_parse(token_function &fx, unsigned type) {
	Token t(_line, 0);
	t.file = _file;
	t.type = type;
	_last = type;
	if (fx) fx(std::move(t));
}

void lexer::_parse(token_function &fx, unsigned type, std::string &&s) {
	Token t(_line, intern(std::move(s)));
	t.file = _file;
	t.type = type;
	_last = type;
	if (fx) fx(std::move(t));
}


template<class T>
void lexer::_parse(token_function &fx, unsigned type, T &&tt) {
	Token t(_line, std::forward<T>(tt));
	t.file = _file;
	t.type = type;
	_last = type;
	if (fx) fx(std::move(t));
}

template<class T>
void lexer::_parse(token_function &fx, unsigned type, std::string &&s, T &&tt) {
	Token t(_line, intern(std::move(s)), std::forward<T>(tt));
	t.file = _file;
	t.type = type;
	_last = type;
	if (fx) fx(std::move(t));
}




unsigned reparse_pragma(const std::string &s) {
%%{
	machine pragma;

	main := 
		  'rtl'i        %{ return p_RTL; }
		| 'rts'i        %{ return p_RTS; }
		| 'rti'i        %{ return p_RTI; }
		| 'cdecl'i      %{ return p_CDECL; }
		| 'pascal'i     %{ return p_PASCAL; }
		| 'stdcall'i    %{ return p_STDCALL; }
		| 'naked'i      %{ return p_NAKED; }
		| 'dynamic'i    %{ return p_DYNAMIC; }
		| 'databank'i   %{ return p_DATABANK; }
		| 'debug'i      %{ return p_DEBUG; }
		| 'segment'i    %{ return p_SEGMENT; }
		| 'noreturn'i   %{ return p_NORETURN; }
		| 'volatile'i   %{ return p_VOLATILE; }
		| 'private'i    %{ return p_PRIVATE; }
		| 'kind'i       %{ return p_KIND; }
		| 'void'i       %{ return p_VOID; }
		| 'locals'i     %{ return p_LOCALS; }
		| 'parameters'i %{ return p_PARAMETERS; }
		| 'return'i     %{ return p_RETURN; }
	;
}%%

%% write data;

	const char *p = s.data();
	const char *pe = s.data() + s.length();
	const char *eof = pe;
	int cs;

%% write init;
%% write exec;

	return 0;
}


char reparse_modifier(const std::string &s) {

	if (s.length() != 1) return 0;
	switch(tolower(s.front())) {
	case 'b': return 'b';
	case 'l': return 'l';
	case 'w': return 'w';
	default:
		return 0;
	}
}


