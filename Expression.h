
#ifndef __expression_h__
#define __expression_h__

#include "common.h"

class Expression {

public:
	static Expression *Integer(uint32_t value);
	static Expression *PC();
	static Expression *Register(dp_register value);
	static Expression *Variable(const std::string *name);
	static Expression *Unary(unsigned op, Expression *a);
	static Expression *Binary(unsigned op, Expression *a, Expression *b);

	static void ErasePool();

	enum expression_type {
		type_unknown = 0,
		type_integer,
		type_register,
		type_pc,
		type_variable,
		type_unary,
		type_binary,
	};

	expression_type type() const {
		return _type;
	}

	bool is_integer() const { return _type == type_integer; }
	bool is_register() const { return _type == type_register; }
	bool is_variable() const { return _type == type_variable; }
	bool is_unary() const { return _type == type_unary; }
	bool is_binary() const { return _type == type_binary; }
	bool is_pc() const { return _type == type_pc; }

	bool is_integer(uint32_t &rv) {
		if (_type == type_integer) {
			rv = int_value;
			return true;
		}
		return false;
	}

	bool is_register(dp_register &rv) {
		if (_type == type_register) {
			rv = register_value;
			return true;
		}
		return false;
	}

	bool is_variable(const std::string * &rv) {
		if (_type == type_variable) {
			rv = string_value;
			return true;
		}
		return false;
	}

	Expression *rename(const std::string *oldname, const std::string *newname);

	Expression *simplify();
	Expression *clone();



protected:
	Expression(expression_type type) : 
		_type(type)
	{}

	Expression(uint32_t value) : 
		_type(type_integer), int_value(value)
	{}
	
	Expression(dp_register value) : 
		_type(type_register), register_value(value)
	{}

	Expression(const std::string *value) : 
		_type(type_variable), string_value(value)
	{}

	Expression(unsigned op, Expression *e) : 
		_type(type_unary), children{e, 0}
	{}

	Expression(unsigned op, Expression *a, Expression *b) : 
		_type(type_binary), children{a, b}
	{}

	~Expression() {}

private:

	Expression *simplify_unary();
	Expression *simplify_binary();
	Expression *clone_unary();
	Expression *clone_binary();

	Expression *rename_unary(const std::string *oldname, const std::string *newname);
	Expression *rename_binary(const std::string *oldname, const std::string *newname);


	expression_type _type = type_unknown;
	Expression *children[2] = {0, 0};
	union {
		const std::string *string_value = 0;
		unsigned op;
		uint32_t int_value;
		dp_register register_value;
	};
};

#endif
