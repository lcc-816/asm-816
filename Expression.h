
#ifndef __expression_h__
#define __expression_h__

#include "common.h"
#include <string>
#include <vector>

enum AddressMode;

class Expression;
class VectorExpression;

typedef Expression *ExpressionPtr;
typedef VectorExpression *VectorExpressionPtr;

typedef const std::string *identifier;

class Expression {

public:
	static ExpressionPtr PC();
	static ExpressionPtr Integer(uint32_t value);
	static ExpressionPtr Register(dp_register value);
	static ExpressionPtr Identifier(identifier name);
	static ExpressionPtr Unary(unsigned op, ExpressionPtr a);
	static ExpressionPtr Binary(unsigned op, ExpressionPtr a, ExpressionPtr b);
	static VectorExpressionPtr Vector();
	static ExpressionPtr String(const std::string *name);

	static void ErasePool();

	enum expression_type {
		type_unknown = 0,
		type_integer,
		type_register,
		type_pc,
		type_identifier,
		type_string,

		type_unary,
		type_binary,
		type_vector,
	};

	expression_type type() const {
		return _type;
	}

	bool is_integer() const { return _type == type_integer; }
	bool is_register() const { return _type == type_register; }
	bool is_identifier() const { return _type == type_identifier; }
	bool is_unary() const { return _type == type_unary; }
	bool is_binary() const { return _type == type_binary; }
	bool is_pc() const { return _type == type_pc; }
	bool is_vector() const {return _type == type_vector; }
	bool is_string() const {return _type == type_string; }

	bool is_terminal() const { return _type < type_unary; }

	bool is_temporary(dp_register &rv) const {
		if (is_register(rv)) {
			return rv.type == 't' || rv.type == 'r';
		}
		return false;
	}


	std::string to_string() const {
		std::string rv;
		to_string(rv);
		return rv;
	}
	std::vector<identifier> identifiers() const {
		std::vector<identifier> rv;
		identifiers(rv);
		return rv;
	}

	std::vector<uint8_t> to_omf(unsigned type, unsigned size) {
		std::vector<uint8_t> rv;

		rv.push_back(type);
		rv.push_back(size);
		to_omf(rv);
		rv.push_back(0x00);
		return rv;
	}

	virtual bool is_identifier(identifier &) const;
	virtual bool is_integer(uint32_t &) const;
	virtual bool is_register(dp_register &) const;

	virtual bool is_string(const std::string *&) const;
	virtual bool is_vector(std::vector<ExpressionPtr> &) const;

	virtual void rename(identifier oldname, identifier newname);
	virtual void rename(dp_register oldreg, dp_register newreg);

	virtual ExpressionPtr simplify();
	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp);

	virtual bool evaluate(uint32_t pc, 
		const std::unordered_map<identifier, uint32_t> &env,
		// something for registers?
		uint32_t &result) const;

protected:
	friend class UnaryExpression;
	friend class BinaryExpression;
	friend class VectorExpression;

	Expression(expression_type type) : _type(type)
	{}

	virtual ~Expression();

	// children override
	virtual void to_string(std::string &) const = 0;
	virtual void identifiers(std::vector<identifier> &) const;

	virtual void to_omf(std::vector<uint8_t> &) const;

private:

	expression_type _type = type_unknown;
};

class VectorExpression : public Expression {

	public:

	size_t size() const { return _children.size(); }
	void push_back(ExpressionPtr e) { _children.push_back(e); }

	virtual bool is_vector(std::vector<ExpressionPtr> &) const final;

	virtual void rename(identifier oldname, identifier newname) final;
	virtual void rename(dp_register oldreg, dp_register newreg) final;

	virtual ExpressionPtr simplify() final;
	//virtual ExpressionPtr simplify(dp_register oldreg, unsigned dp) final;


	protected:

	VectorExpression() : Expression(type_vector)
	{}

	virtual ~VectorExpression() final;

	virtual void to_string(std::string &) const final;
	virtual void identifiers(std::vector<identifier> &) const final;

	private:
	friend class Expression;
	

	std::vector<ExpressionPtr> _children;
};

#endif

