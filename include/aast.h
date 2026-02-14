#ifndef __AAST_H__
#define __AAST_H__
#include "ast.h"
#include <iostream>
#include <string>
#include <vector>

struct Operand {
public:
  virtual ~Operand() = default;

protected:
  virtual void write(std::ostream &ostr) const = 0;
  friend std::ostream &operator<<(std::ostream &ostr, const Operand &opr) {
    opr.write(ostr);
    return ostr;
  }
};

struct Imm : Operand {
public:
  int val;
  Imm(int val_) : val(val_) {}

protected:
  void write(std::ostream &ostr) const override { ostr << "$" << val; }
};

struct Register : Operand {
public:
  std::string name;
  Register() { name = "\%eax"; }

protected:
  void write(std::ostream &ostr) const override { ostr << name; }
};

struct Instruction {
public:
  virtual ~Instruction() = default;

protected:
  virtual void write(std::ostream &ostr) const = 0;
  friend std::ostream &operator<<(std::ostream &ostr,
                                   const Instruction &opr) {
    opr.write(ostr);
    return ostr;
  }
};

struct Mov : Instruction {
public:
  Operand *src, *dst;
  Mov(Operand *src_, Operand *dst_) : src(src_), dst(dst_) {}
  ~Mov() {
    delete src;
    delete dst;
  }

protected:
  void write(std::ostream &ostr) const override {
    ostr << "movl" << "\t" << *src << ", " << *dst;
  }
};

struct Ret : Instruction {
public:
  std::string name;
  Ret() { name = "ret"; }

protected:
  void write(std::ostream &ostr) const override { ostr << name; }
};

struct AIdentifier {
  std::string name;
  AIdentifier(std::string &name_) : name(name_) {}
  friend std::ostream &operator<<(std::ostream &ostr,
                                   const Identifier &identifier);
};

struct AFunction {
  AIdentifier *name;
  std::vector<Instruction *> instructions;
  AFunction(AIdentifier *name_, std::vector<Instruction *> &instructions_)
      : name(name_), instructions(instructions_) {}
  ~AFunction() {
    delete name;
    for (Instruction *instr : instructions) {
      delete instr;
    }
  }
  friend std::ostream &operator<<(std::ostream &ostr,
                                   const AFunction &function);
};

struct AProgram {
  AFunction *function_definition;
  AProgram(AFunction *function_definition_)
      : function_definition(function_definition_) {}
  ~AProgram() { delete function_definition; }
  friend std::ostream &operator<<(std::ostream &ostr,
                                   const AProgram &program);
};

#endif // !__AAST_H__
