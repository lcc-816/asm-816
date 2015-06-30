
#ifndef __expression_h__
#define __expression_h__

//#include "common.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

#include "dp_register.h"
#include "intern.h"

//enum AddressMode;

class Expression;
class VectorExpression;

typedef Expression *ExpressionPtr;
typedef VectorExpression *VectorExpressionPtr;

typedef const std::string *identifier;
typedef std::unordered_map<identifier, uint32_t> identifier_map;


class Expression {

public:


	static ExpressionPtr PC();
	static ExpressionPtr Integer(uint32_t value);
	static ExpressionPtr Register(dp_register value);
	static ExpressionPtr Rel(uint32_t offset);

	static ExpressionPtr Identifier(identifier name);
	static ExpressionPtr Identifier(std::string &&s) { return Identifier(intern(std::move(s))); }
	static ExpressionPtr Identifier(const std::string &s) { return Identifier(intern(s)); }

	static ExpressionPtr Unary(unsigned op, ExpressionPtr a);
	static ExpressionPtr Binary(unsigned op, ExpressionPtr a, ExpressionPtr b);
	static VectorExpressionPtr Vector();
	static VectorExpressionPtr Vector(const std::vector<ExpressionPtr> &);
	static VectorExpressionPtr Vector(std::vector<ExpressionPtr> &&);

	static ExpressionPtr String(const std::string *name);

	static void ErasePool();

	enum expression_type {
		type_unknown = 0,
		type_integer,
		type_register,
		type_pc,
		type_identifier,
		type_string,
		type_rel,

		type_unary,
		type_binary,
		type_vector,
	};

	expression_type type() const {
		return _type;
	}

	bool is_binary() const { return _type == type_binary; }
	bool is_identifier() const { return _type == type_identifier; }
	bool is_integer() const { return _type == type_integer; }
	bool is_pc() const { return _type == type_pc; }
	bool is_register() const { return _type == type_register; }
	bool is_rel() const {return _type == type_rel; }
	bool is_string() const {return _type == type_string; }
	bool is_unary() const { return _type == type_unary; }
	bool is_vector() const {return _type == type_vector; }

	bool is_terminal() const { return _type < type_unary; }

	bool is_temporary(dp_register &rv) const {
		if (is_register(rv)) {
			return rv.is_temporary();
		}
		return false;
	}


	std::string to_string() const {
		std::string rv;
		to_string(rv);
		return rv;
	}

	std::vector<identifier> identifiers() const;


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
	virtual bool is_rel(uint32_t &) const;

	virtual bool is_string(const std::string *&) const;
	virtual bool is_vector(std::vector<ExpressionPtr> &) const;

	ExpressionPtr rename(identifier oldname, identifier newname);
	ExpressionPtr rename(dp_register oldname, dp_register newname);

	ExpressionPtr make_relative(uint32_t pc) /* const */;
	ExpressionPtr make_relative(uint32_t pc, const identifier_map &env) /* const */;

	ExpressionPtr simplify();

	struct register_info {
		std::unordered_map<dp_register, uint32_t> trmap;
		uint32_t vbase = 0;
		uint32_t pbase = 0;
	};
	ExpressionPtr assign_registers(const register_info &ri);

	bool evaluate(uint32_t pc, const identifier_map &env, uint32_t &result) const;
	bool evaluate_relative(uint32_t pc, int32_t &result) const;

	virtual uint32_t evaluate(uint32_t pc, const identifier_map &env) const /* throw(std::runtime_exception) */;
	virtual int32_t evaluate_relative(uint32_t pc) const /* throw(std::runtime_exception) */;


	class Visitor;
	class ConstVisitor;
	class MapVisitor;

protected:
	friend class UnaryExpression;
	friend class BinaryExpression;
	friend class VectorExpression;


	virtual void accept(Visitor &) = 0;
	virtual void accept(ConstVisitor &) const= 0;
	virtual ExpressionPtr accept(MapVisitor &) const = 0;



	Expression(expression_type type) : _type(type)
	{}

	virtual ~Expression();

	// children override
	virtual void to_string(std::string &) const = 0;

	virtual void to_omf(std::vector<uint8_t> &) const;

private:

	expression_type _type = type_unknown;
};

class VectorExpression : public Expression {

	public:
	typedef std::vector<ExpressionPtr> children_type;

	size_t size() const { return _children.size(); }
	void push_back(ExpressionPtr e) { _children.push_back(e); }

	virtual bool is_vector(std::vector<ExpressionPtr> &) const override final;

	protected:

	virtual void accept(Visitor &) override final;
	virtual void accept(ConstVisitor &) const override final;
	virtual ExpressionPtr accept(MapVisitor &) const override final;

	VectorExpression() : Expression(type_vector)
	{}

	VectorExpression(std::vector<ExpressionPtr> &&);
	VectorExpression(const std::vector<ExpressionPtr> &);

	virtual ~VectorExpression() final;

	virtual void to_string(std::string &) const final;

	private:
	friend class Expression;
	

	children_type _children;
};

#endif

