#ifndef __AST_H__
#define __AST_H__
#include <iostream>
#include <string>

struct Unary_Operator {
public:
  virtual ~Unary_Operator() = default;
protected:
  std::string symbol;
  friend std::ostream &operator<<(std::ostream&ostr, const Unary_Operator &unary_operator) {
    ostr << "Unary Operator(" << unary_operator.symbol << ")";
    return ostr;
  }
};

struct Complement : Unary_Operator {
  Complement() { symbol = "~"; }
};

struct Negate : Unary_Operator {
  Negate() { symbol = "-"; }
};

struct Unary_Not : Unary_Operator {
   Unary_Not() { symbol = "!"; }
};

struct Binary_Operator {
public:
  virtual ~Binary_Operator() = default;
protected:
  std::string symbol;
  friend std::ostream &operator<<(std::ostream&ostr, const Binary_Operator &binary_operator) {
    ostr << binary_operator.symbol;
    return ostr;
  }
};

struct Add : Binary_Operator {
  Add() { symbol = "+"; }
};

struct Subtract : Binary_Operator {
  Subtract() { symbol = "-"; }
};

struct Multiply : Binary_Operator {
  Multiply() { symbol = "*"; }
};

struct Divide : Binary_Operator {
  Divide() { symbol = "/"; }
};

struct Remainder : Binary_Operator {
  Remainder() { symbol = "%"; }
};

struct And : Binary_Operator{
   And() { symbol = "&&"; }
};

struct Or : Binary_Operator{
   Or() { symbol = "||";}
};

struct Equal : Binary_Operator{
   Equal() { symbol = "=="; }
};

struct NotEqual : Binary_Operator{
   NotEqual() { symbol = "!="; }
};

struct LessThan : Binary_Operator{
   LessThan() { symbol = "<"; }
};

struct LessOrEqual : Binary_Operator{
   LessOrEqual() { symbol = "<="; }
};

struct GreaterThan : Binary_Operator{
   GreaterThan() { symbol = ">"; }
};

struct GreaterOrEqual : Binary_Operator{
   GreaterOrEqual() { symbol = ">="; }
};

struct Expression {
public:
  virtual ~Expression() = default;

protected:
  virtual void print(std::ostream &ostr) const = 0;
  friend std::ostream &operator<<(std::ostream &ostr, const Expression &exp) {
    exp.print(ostr);
    return ostr;
  }
};

struct Constant : Expression {
public:
  int val;
  Constant(int val_) : val(val_) {}

protected:
  void print(std::ostream &ostr) const override {
    ostr << "Constant(" << val << ")";
  }
};

struct Unary : Expression {
public:
  Unary_Operator *unary_operator;
  Expression *exp;
  Unary(Unary_Operator *unary_operator_, Expression *exp_)
      : unary_operator(unary_operator_), exp(exp_) {}
  ~Unary() {
    delete unary_operator;
    delete exp;
  }
protected:
  void print(std::ostream &ostr) const override {
    ostr << "Unary(" << *unary_operator << ", " << *exp << ")";
  }
};

struct Binary : Expression {
public:
  Binary_Operator *binary_operator;
  Expression *left, *right;
  Binary(Binary_Operator *binary_operator_, Expression *left_, Expression *right_): binary_operator(binary_operator_), left(left_), right(right_) {}
  ~Binary() {
    delete binary_operator;
    delete left;
    delete right;
  }
  protected:
  void print(std::ostream &ostr) const override {
    ostr << "Binary Operator(" << *binary_operator << ", " << *left << ", " << *right;
  }
};

struct Identifier {
  std::string name;
  Identifier(std::string name_) : name(name_) {}
  friend std::ostream &operator<<(std::ostream &ostr,
                                  const Identifier &identifier);
};

struct Statement {
public:
  virtual ~Statement() = default;

protected:
  virtual void print(std::ostream &ostr) const = 0;
  friend std::ostream &operator<<(std::ostream &ostr, const Statement &ret) {
    ret.print(ostr);
    return ostr;
  }
};

struct Return : Statement {
public:
  Expression *exp;
  Return(Expression *exp_) : exp(exp_) {}
  ~Return() { delete exp; }

protected:
  void print(std::ostream &ostr) const override {
    ostr << "Return(" << std::endl;
    ostr << "\t\t\t" << *exp << std::endl;
    ostr << "\t\t)";
  }
};

struct Function {
  Identifier *name;
  Statement *body;
  Function(Identifier *name_, Statement *body_) : name(name_), body(body_) {}
  ~Function() {
    delete name;
    delete body;
  }
  friend std::ostream &operator<<(std::ostream &ostr, const Function &function);
};

struct Program {
  Function *func;
  Program(Function *function_definition) : func(function_definition) {}
  ~Program() { delete func; }
  friend std::ostream &operator<<(std::ostream &ostr, const Program &program);
};

#endif // !__AST_H__
