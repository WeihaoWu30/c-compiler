#ifndef __RETURN_H__
#define __RETURN_H__
#include "expression.h"
#include "statement.h"
#include <iostream>

class Statement;
class Expression;

class Return : public Statement {
public:
  Return(Expression *exp_) : exp(exp_) {}
  ~Return() { delete exp; }

protected:
	void print(std::ostream& ostr) const override {
		ostr << "Return(" << std::endl;
		ostr << "\t\t\t" << *exp;
		ostr << "\t\t)";
	}

private:
  Expression *exp;
};

#endif
