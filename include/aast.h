#ifndef __AAST_H__
#define __AAST_H__
#include "ast.h"
#include <iostream>
#include <string>
#include <list>

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

struct RegType {
public:
  virtual ~RegType() = default;
protected:
  std::string name;
};

struct AX : RegType {
  AX() { name = "%eax"; }
};
struct R10 : RegType {
  R10() { name = "%r10d"; }
};

struct Reg : Operand {
public:
  RegType* reg;
  Reg(RegType* reg_): reg(reg_) {}
  ~Reg() { delete reg; }
protected:
  void write(std::ostream &ostr) const override { 
    
  }
};

struct AIdentifier {
  std::string name;
  AIdentifier(std::string &name_) : name(name_) {}
  friend std::ostream &operator<<(std::ostream &ostr,
                                   const Identifier &identifier);
};

struct Pseudo : Operand {
public:
  AIdentifier* identifier;
  Pseudo(AIdentifier* identifier_): identifier(identifier_){}
  ~Pseudo() { delete identifier; }
protected:
  void write(std::ostream &ostr) const override {};
};

struct Stack : Operand {
public:
  int offset;
  Stack(int offset_): offset(offset_){}
protected:
  void write(std::ostream &ostr) const override {}
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

struct AUnary_Operator{
   virtual ~AUnary_Operator() = default;
};

struct Neg: AUnary_Operator{};
struct Not: AUnary_Operator{};

struct AUnary : Instruction {
public:
   AUnary_Operator *unary_operator;
   Operand* operand;
   AUnary(AUnary_Operator *unary_operator_, Operand* operand_): unary_operator(unary_operator_), operand(operand_){}
   ~AUnary() {
    delete unary_operator;
    delete operand;
   }
protected:
  void write(std::ostream &ostr) const override {}
};

struct AllocateStack : Instruction {
public:
   int bytes;
   AllocateStack(int bytes_): bytes(bytes_){}
protected:
  void write(std::ostream& ostr) const override {}
};

struct Ret : Instruction {
public:
  std::string name;
  Ret() { name = "ret"; }

protected:
  void write(std::ostream &ostr) const override { ostr << name; }
};

struct AFunction {
  AIdentifier *name;
  std::list<Instruction *> instructions;
  AFunction(AIdentifier *name_, std::list<Instruction *> &instructions_)
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
