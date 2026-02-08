#ifndef __CONSTANT_H___
#define __CONSTANT_H___
#include "expression.h"

class Expression;

class Constant: public Expression {
public:
    Constant(int val_): val(val_) {} 
    int get_value() { return val; }
    friend std::ostream& operator<<(std::ostream& ostr, const Constant& constant);
private:
    int val;
};

#endif