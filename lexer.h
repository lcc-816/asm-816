#ifndef __lexer_h__
#define __lexer_h__

#include <vector>
#include <cstdint>
#include <functional>

#include "token.h"


class lexer {
public:
	lexer(identifier file = nullptr);
	lexer(const std::string &file);
	lexer(std::string &&file);

	~lexer() = default;
	lexer(const lexer &) = delete;
	lexer(lexer &&) = delete;

	lexer &operator=(const lexer &) = delete;
	lexer &operator=(lexer &&) = delete;

	int line() const { return _line; }
	int error_count() const { return _error; }

	typedef std::function<void(Token &&)> token_function;
	typedef std::vector< Token > token_vector;

	token_vector tokenize(const void *begin,  const void *end, bool is_eof);
	void tokenize(const void *begin, const void *end, bool is_eof, token_function fx);


private:

	void _parse(token_function &, unsigned);
	void _parse(token_function &, unsigned, std::string &&s);

	template<class T>
	void _parse(token_function &, unsigned type, T &&t);

	template<class T>
	void _parse(token_function &, unsigned type, std::string &&s, T &&t);

	int act = 0;
	int cs = 0;
	int _line = 1;
	identifier _file = nullptr;
	unsigned _last = 0;
	unsigned _error = 0;

	std::vector<uint8_t> buffer;	
};

#endif
