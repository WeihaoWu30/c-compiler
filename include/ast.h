#ifndef __AST_H__
#define __AST_H__
#include <iostream>
#include <string>

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
  friend std::ostream &operator<<(std::ostream &ostr, const Constant &constant);

protected:
  void print(std::ostream &ostr) const override {
    ostr << "Constant(" << val << ")" << std::endl;
  }
};

struct Identifier {
  std::string name;
  Identifier(std::string name_) : name(name_) {}
  friend std::ostream &operator<<(std::ostream &ostr,
                                  const Identifier &identifier);
};

struct Return : Statement {
public:
  Expression *exp;
  Return(Expression *exp_) : exp(exp_) {}
  ~Return() { delete exp; }

protected:
  void print(std::ostream &ostr) const override {
    ostr << "Return(" << std::endl;
    ostr << "\t\t\t" << *exp;
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
