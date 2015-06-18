#ifndef __expression_internal_h__
#define __expression_internal_h__

#include "Expression.h"
#include <string>
#include <cstdint>

namespace Expression_Internal {

	uint32_t unary_op(unsigned op, uint32_t);
	uint32_t binary_op(unsigned op, uint32_t, uint32_t);

	std::string int_to_string(uint32_t);

}

class BinaryExpression;
class IdentifierExpression;
class IntegerExpression;
class PCExpression;
class RelExpression;
class RegisterExpression;
class StringExpression;
class UnaryExpression;

class Expression::Visitor {
public:
	virtual ~Visitor();
	virtual void visit(BinaryExpression &);
	virtual void visit(IdentifierExpression &);
	virtual void visit(IntegerExpression &);
	virtual void visit(PCExpression &);
	virtual void visit(RegisterExpression &);
	virtual void visit(RelExpression &);
	virtual void visit(StringExpression &);
	virtual void visit(UnaryExpression &);
	virtual void visit(VectorExpression &);
};

class Expression::ConstVisitor {
public:
	virtual ~ConstVisitor();
	virtual void visit(const BinaryExpression &);
	virtual void visit(const IdentifierExpression &);
	virtual void visit(const IntegerExpression &);
	virtual void visit(const PCExpression &);
	virtual void visit(const RegisterExpression &);
	virtual void visit(const RelExpression &);
	virtual void visit(const StringExpression &);
	virtual void visit(const UnaryExpression &);
	virtual void visit(const VectorExpression &);
};


class Expression::MapVisitor {
public:
	virtual ~MapVisitor();
	virtual ExpressionPtr visit(BinaryExpression &);
	virtual ExpressionPtr visit(IdentifierExpression &);
	virtual ExpressionPtr visit(IntegerExpression &);
	virtual ExpressionPtr visit(PCExpression &);
	virtual ExpressionPtr visit(RegisterExpression &);
	virtual ExpressionPtr visit(RelExpression &);
	virtual ExpressionPtr visit(StringExpression &);
	virtual ExpressionPtr visit(UnaryExpression &);
	virtual ExpressionPtr visit(VectorExpression &);

};


class PCExpression : public Expression {
public:
	PCExpression() : Expression(type_pc)
	{}

	virtual void to_string(std::string &) const final;

	// virtual bool evaluate(uint32_t pc, 
	// 	const identifier_map &env,
	// 	uint32_t &result) const final;

	virtual uint32_t evaluate(uint32_t pc, 
		const identifier_map &env) const final;


	virtual void to_omf(std::vector<uint8_t> &) const final;

	//virtual void set_pc(uint32_t pc) final;
	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:

};


class RelExpression : public Expression {
public:
	RelExpression(uint32_t offset) : Expression(type_pc), _offset(offset)
	{}

	virtual bool is_rel(uint32_t &) const final;
	virtual void to_string(std::string &) const final;


	virtual void to_omf(std::vector<uint8_t> &) const final;

	virtual uint32_t evaluate(uint32_t pc, 
		const identifier_map &env) const final;


	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
		uint32_t _offset = 0;
};



class IntegerExpression : public Expression {
public:
	IntegerExpression(uint32_t value) : 
	Expression(type_integer), _value(value)
	{}

	virtual bool is_integer(uint32_t &) const final;
	virtual void to_string(std::string &) const final;

	// virtual bool evaluate(uint32_t pc, 
	// 	const identifier_map &env,
	// 	uint32_t &result) const final;

	virtual uint32_t evaluate(uint32_t pc, 
		const identifier_map &env) const final;


	virtual void to_omf(std::vector<uint8_t> &) const final;
	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	uint32_t _value;
};




class RegisterExpression : public Expression {
public:
	RegisterExpression(dp_register value) : 
	Expression(type_register), _value(value)
	{}

	virtual bool is_register(dp_register &) const final;
	virtual void to_string(std::string &) const final;
	virtual void rename(dp_register, dp_register) final;

	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp) final;
	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	dp_register _value;
};




class IdentifierExpression : public Expression {
public:
	IdentifierExpression(identifier value) : Expression(type_identifier), _value(value)
	{}

	virtual bool is_identifier(identifier &) const final;
	virtual void to_string(std::string &) const final;

	//virtual void identifiers(std::vector<identifier> &) const final;
	virtual void rename(identifier, identifier) final;	

	// virtual bool evaluate(uint32_t pc, 
	// 	const identifier_map &env,
	// 	uint32_t &result) const final;

	virtual uint32_t evaluate(uint32_t pc, 
		const identifier_map &env) const final;


	virtual void to_omf(std::vector<uint8_t> &) const final;
	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	identifier _value;
};


class StringExpression : public Expression {
public:
	StringExpression(identifier value) : Expression(type_string), _value(value)
	{}

	virtual bool is_string(identifier &) const final;
	virtual void to_string(std::string &) const final;
	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	identifier _value;
};



class UnaryExpression : public Expression {
public:
	typedef std::array<ExpressionPtr, 1> children_type;

	UnaryExpression(unsigned op, ExpressionPtr a) :
	Expression(type_unary), _op(op), _children{a}
	{}

	virtual void to_string(std::string &) const final;
	//virtual void identifiers(std::vector<identifier> &) const final;

	virtual void rename(identifier, identifier) final;
	virtual void rename(dp_register, dp_register) final;
	virtual ExpressionPtr simplify() final;
	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp) final;

	// virtual bool evaluate(uint32_t pc, 
	// 	const identifier_map &env,
	// 	uint32_t &result) const final;

	virtual uint32_t evaluate(uint32_t pc, 
		const identifier_map &env) const final;


	virtual void to_omf(std::vector<uint8_t> &) const final;

	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	friend class SimplifyVisitor;
	
	unsigned _op;
	children_type _children;
};



class BinaryExpression : public Expression {
public:
	typedef std::array<ExpressionPtr, 2> children_type;

	BinaryExpression(unsigned op, ExpressionPtr a, ExpressionPtr b) :
	Expression(type_binary), _op(op), _children{a, b}
	{}

	virtual void to_string(std::string &) const final;
	//virtual void identifiers(std::vector<identifier> &rv) const final;

	virtual void rename(identifier, identifier) final;
	virtual void rename(dp_register, dp_register) final;
	virtual ExpressionPtr simplify() final;
	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp) final;

	// virtual bool evaluate(uint32_t pc, 
	// 	const identifier_map &env,
	// 	uint32_t &result) const final;

	virtual uint32_t evaluate(uint32_t pc, 
		const identifier_map &env) const final;


	virtual void to_omf(std::vector<uint8_t> &) const final;

	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const final;
	virtual ExpressionPtr accept(MapVisitor &) final;

private:
	friend class SimplifyVisitor;

	unsigned _op;
	children_type _children;
};


#endif
