
class Expression;
typedef std::shared_ptr<Expression> ExpressionPtr;

class Expression {

	enum {
		TypeInteger = 1,
		TypeUnary,
		TypeBinary,
		TypeVariable,
		TypeRegister
	};

	unsigned type() const {
		return _type;
	}

	bool is_integer() const { return _type = TypeInteger; }
	bool is_register() const { return _type == TypeRegister; }
	bool is_variable() const { return _type == TypeVariable; }
	bool is_unary() const { return _type == TypeUnary; }

	void simplify();


	virtual ~Expression();
	virtual bool operator==(const Expression &rhs);

protected:
	Expression(unsigned type) : _type(type)
	{}

private:
	unsigned _type;
};


class IntegerExpression : public Expression {
	uint32_t value;

	static IntegerExpression *New(uint32_t value) {
		IntegerExpression *e = new IntegerExpression(value);
		Arena.push_back(e);
		return e;
	}

	virtual bool operator==(const Expression &rhs) {
		if (&rhs == this) return true;

		if (rhs.type() != type()) return false;

		const IntegerExpression &e = reinterpret_cast<IntegerExpression &>(rhs);

		return (e.value == value);
	}

	virtual Expression *simplify() {
		return *this;
	}
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
	typedef UnaryExpression Type;

	unsigned op;
	ExpressionPtr e;

	virtual bool operator==(const Expression &rhs) {
		if (&rhs == this) return true;
		if (rhs.type() != type()) return false;

		Type &other = static_cast<Type &>(rhs);

		return (other.op == op && other->e == e);
	}


	virtual Expression *simplify() {
		child = simplify(child);
		IntegerExpression e = e->as_integer();
		if (!e) return this;

		if (e->is_integer()) {
			uint32_t value = e->value();
			switch(op){
				case '-': value = -value; break;
				case '+': value = +value; break;
				case '!': value = !value; break;
				case '~': value = ~value; break;
			}
			return IntegerExpression::New(value);
		}
		return this;
	}
};


class BinaryExpression : public Expression {
	unsigned op;
	ExpressionPtr lhs;
	ExpressionPtr rhs;
};