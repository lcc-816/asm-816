#ifndef __expression_internal_h__
#define __expression_internal_h__

#include "Expression.h"
#include <string>
#include <cstdint>
#include <memory>
#include <array>

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
	virtual ExpressionPtr visit(const BinaryExpression &);
	virtual ExpressionPtr visit(const IdentifierExpression &);
	virtual ExpressionPtr visit(const IntegerExpression &);
	virtual ExpressionPtr visit(const PCExpression &);
	virtual ExpressionPtr visit(const RegisterExpression &);
	virtual ExpressionPtr visit(const RelExpression &);
	virtual ExpressionPtr visit(const StringExpression &);
	virtual ExpressionPtr visit(const UnaryExpression &);
	virtual ExpressionPtr visit(const VectorExpression &);

};

class SimplifyVisitor : public Expression::MapVisitor {
public:

	virtual ExpressionPtr visit(const BinaryExpression &) override;
	virtual ExpressionPtr visit(const UnaryExpression &) override;

};

class PCExpression : public Expression {
public:
	PCExpression() : Expression(type_pc)
	{}

	virtual void to_string(std::string &) const override final;

	virtual uint32_t evaluate(uint32_t pc, const identifier_map &env) const override final;

	virtual void to_omf(std::vector<uint8_t> &) const override final;

	virtual void accept(Visitor &) override final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;

private:

};


class RelExpression : public Expression {
public:
	RelExpression(uint32_t offset) : Expression(type_pc), _offset(offset)
	{}

	virtual bool is_rel(uint32_t &) const override final;
	virtual void to_string(std::string &) const override final;


	virtual void to_omf(std::vector<uint8_t> &) const override final;

	virtual uint32_t evaluate(uint32_t pc, const identifier_map &env) const override final;

	virtual int32_t evaluate_relative(uint32_t pc) const override final;

	virtual void accept(Visitor &) override final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;

	// rel and pc are never ==.
	//virtual bool operator==(const Expression &) const override final;
	//bool operator==(const RelExpression &) const;

private:
		uint32_t _offset = 0;
};



class IntegerExpression : public Expression {
public:
	IntegerExpression(uint32_t value) : 
	Expression(type_integer), _value(value)
	{}

	virtual bool is_integer(uint32_t &) const override final;
	virtual void to_string(std::string &) const override final;

	virtual uint32_t evaluate(uint32_t pc, const identifier_map &env) const override final;


	virtual void to_omf(std::vector<uint8_t> &) const override final;
	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;

	virtual bool operator==(const Expression &) const override final;
	bool operator==(const IntegerExpression &) const;

private:
	uint32_t _value;
};




class RegisterExpression : public Expression {
public:
	RegisterExpression(dp_register value) : 
	Expression(type_register), _value(value)
	{}

	virtual bool is_register(dp_register &) const override final;
	virtual void to_string(std::string &) const override final;
	virtual void accept(Visitor &) final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;

	virtual bool operator==(const Expression &) const override final;
	bool operator==(const RegisterExpression &) const;

private:
	dp_register _value;
};




class IdentifierExpression : public Expression {
public:
	IdentifierExpression(identifier value, bool weak = false) : Expression(type_identifier), _value(value), _weak(weak)
	{}

	virtual bool is_identifier(identifier &) const override final;
	virtual void to_string(std::string &) const override final;

	virtual uint32_t evaluate(uint32_t pc, const identifier_map &env) const override final;


	virtual void to_omf(std::vector<uint8_t> &) const override final;
	virtual void accept(Visitor &) override final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;


	virtual bool operator==(const Expression &) const override final;
	bool operator==(const IdentifierExpression &) const;

private:
	identifier _value;
	bool _weak = false;
};


class StringExpression : public Expression {
public:
	StringExpression(identifier value) : Expression(type_string), _value(value)
	{}

	virtual bool is_string(identifier &) const override final;
	virtual void to_string(std::string &) const override final;
	virtual void accept(Visitor &) override final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;


	virtual bool operator==(const Expression &) const override final;
	bool operator==(const StringExpression &) const;

private:
	identifier _value;
};



class UnaryExpression : public Expression {
public:
	typedef std::array<ExpressionPtr, 1> children_type;

	UnaryExpression(unsigned op, ExpressionPtr a) :
	Expression(type_unary), _op(op), _children{{a}}
	{}

	virtual void to_string(std::string &) const override final;

	virtual uint32_t evaluate(uint32_t pc, const identifier_map &env) const override final;

	virtual bool operator==(const Expression &) const override final;
	bool operator==(const UnaryExpression &) const;

	virtual void to_omf(std::vector<uint8_t> &) const override final;

	virtual void accept(Visitor &) override final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;

private:
	friend class SimplifyVisitor;
	
	ExpressionPtr simplify_me() const;

	unsigned _op;
	children_type _children;
};



class BinaryExpression : public Expression {
public:
	typedef std::array<ExpressionPtr, 2> children_type;

	BinaryExpression(unsigned op, ExpressionPtr a, ExpressionPtr b) :
	Expression(type_binary), _op(op), _children{{a, b}}
	{}

	virtual void to_string(std::string &) const override final;

	virtual uint32_t evaluate(uint32_t pc, const identifier_map &env) const final;

	virtual bool operator==(const Expression &) const override final;
	bool operator==(const BinaryExpression &) const;

	virtual void to_omf(std::vector<uint8_t> &) const override final;

	virtual void accept(Visitor &) override final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;

private:
	friend class SimplifyVisitor;

	ExpressionPtr simplify_me() const;

	unsigned _op;
	children_type _children;
};

// will change if I start using shared_ptr<>
inline ExpressionPtr to_expression_ptr(const Expression &e) {
	//return std::addressof(const_cast<Expression &>(e));
	return const_cast<Expression &>(e).shared_from_this();
}

inline ExpressionPtr to_expression_ptr(Expression &e) {
	return e.shared_from_this();
	//return std::addressof(e);
}


#endif
