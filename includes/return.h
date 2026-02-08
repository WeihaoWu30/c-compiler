#ifndef RETURN_H
#define RETURN_H
#include "statement.h"
#include "expression.h"

class Statement;
class Expression;

class Return: public Statement{
public:
    Return(Expression* exp_): exp(exp_) {}
    ~Return() { delete exp; }
    friend std::ostream& operator<<(std::ostream& ostr, const return& ret);
private:
    Expression* exp;
};

#endif