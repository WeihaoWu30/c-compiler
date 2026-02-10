#ifndef __CONSTANT_H__
#define __CONSTANT_H__
#include "expression.h"
#include <iostream>

class Expression;

class Constant : public Expression {
public:
  Constant(int val_) : val(val_) {}
  int get_value() { return val; }
  friend std::ostream &operator<<(std::ostream &ostr, const Constant &constant);

private:
  int val;

protected:
  void print(std::ostream &ostr) const override {
    ostr << "Constant(" << val << ")" << std::endl;
  }
};

#endif
