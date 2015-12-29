#ifndef __symbol_table_h__
#define __symbol_table_h__

#include <vector>
#include <unordered_map>

#include "types.h"

class symbol_table {
	
public:

	symbol_table();
	symbol_table(symbol_table &&) = default;
	symbol_table(const symbol_table &) = default;

	symbol_table& operator=(symbol_table &&) = default;
	symbol_table& operator=(const symbol_table &) = default;


	ExpressionPtr find(identifier key);

	void insert(identifier key, ExpressionPtr value);

	void push();
	void pop();

private:
	std::vector< std::unordered_map<identifier, ExpressionPtr> > _spine;
};

#endif