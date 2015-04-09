
#ifndef __expression_h__
#define __expression_h__

#include "common.h"
#include <string>
#include <vector>

enum AddressMode;

class Expression {

public:
	static Expression *Integer(uint32_t value);
	static Expression *PC();
	static Expression *Register(dp_register value);
	static Expression *Variable(const std::string *name);
	static Expression *Unary(unsigned op, Expression *a);
	static Expression *Binary(unsigned op, Expression *a, Expression *b);
	static Expression *Vector(Expression *child);
	static Expression *Vector(Expression **children, unsigned count);
	static Expression *String(const std::string *name);

	static void ErasePool();

	enum expression_type {
		type_unknown = 0,
		type_integer,
		type_register,
		type_pc,
		type_variable,
		type_unary,
		type_binary,
		type_vector,
		type_string,
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
	bool is_vector() const {return _type == type_vector; }
	bool is_string() const {return _type == type_string; }

	std::vector<const std::string *> collect_variables();

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

	bool is_string(const std::string * &rv) {
		if (_type == type_string) {
			rv = string_value;
			return true;
		}
		return false;
	}

	bool is_vector(std::vector<Expression *> &rv) {
		if (_type == type_vector) {
			rv = vector_value;
			return true;
		}
		rv.clear();
		return false;
	}

	Expression *append(Expression *child);

	Expression *rename(const std::string *oldname, const std::string *newname);
	Expression *rename(dp_register oldreg, dp_register newreg);

	// currently, register is top-level only -- not part of an expression.

	/*
	 * void collect_variables(variable_set);
	 * void collect_registers(register_set); //?
	 */


	Expression *simplify();
	Expression *clone();

	std::string to_string() const;


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

	Expression(expression_type type, const std::string *value) : 
		_type(type), string_value(value)
	{}

	Expression(unsigned o, Expression *e) : 
		_type(type_unary), op(o), children{e, 0}
	{}

	Expression(unsigned o, Expression *a, Expression *b) : 
		_type(type_binary), op(o), children{a, b}
	{}

	~Expression() {
		if (_type == type_vector) {
			vector_value.~vector();
		}
	}

private:

	void append_vector_element(Expression *);

	Expression *simplify_unary();
	Expression *simplify_binary();
	Expression *simplify_vector();

	Expression *clone_unary();
	Expression *clone_binary();
	Expression *clone_vector();

	Expression *rename_unary(const std::string *oldname, const std::string *newname);
	Expression *rename_binary(const std::string *oldname, const std::string *newname);
	Expression *rename_vector(const std::string *oldname, const std::string *newname);

	std::string to_string_unary() const;
	std::string to_string_binary() const;
	std::string to_string_vector() const;


	void collect_variables(std::vector<const std::string *> &);
	void collect_variables_unary(std::vector<const std::string *> &);
	void collect_variables_binary(std::vector<const std::string *> &);
	void collect_variables_vector(std::vector<const std::string *> &);

	expression_type _type = type_unknown;
	Expression *children[2] = {0, 0};
	union {
		const std::string *string_value = 0;
		unsigned op;
		uint32_t int_value;
		dp_register register_value;
		std::vector<Expression *> vector_value;

	};
};

#endif
