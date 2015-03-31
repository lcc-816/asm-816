
class Expression {

	virtual ~Expression();
	virtual bool is_integer() const;
	virtual bool is_register() const;
	virtual bool is_variable() const;

	virtual void to_omf();
	void simplify();

};

typedef std::shared_ptr<Expression> ExpressionPtr;

class IntegerExpression : public Expression {
	uint32_t value;
};

// includes pc - *
class VariableExpression : public Expression {
	const std::string *value;
};

class RegisterExpression : public Expression {
	character type;
	unsigned value;
};

class UnaryExpression : public Expression {
	unsigned op;
	ExpressionPtr e;
};

class BinaryExpression : public Expression {
	unsigned op;
	ExpressionPtr lhs;
	ExpressionPtr rhs;
};