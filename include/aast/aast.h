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

struct RegType {
public:
  virtual ~RegType() = default;
protected:
  std::string name;
  friend std::ostream &operator<<(std::ostream &ostr, const RegType& regType) {
    ostr << regType.name;
    return ostr;
  }
};

struct AInstruction {
public:
  virtual ~AInstruction() = default;

protected:
  virtual void write(std::ostream &ostr) const = 0;
  friend std::ostream &operator<<(std::ostream &ostr,
                                   const AInstruction &opr) {
    opr.write(ostr);
    return ostr;
  }
};

struct AUnary_Operator {
public:
  virtual ~AUnary_Operator() = default;
protected:
  std::string instruction;
  friend std::ostream &operator<<(std::ostream &ostr, const AUnary_Operator& unary_operator) {
    ostr << unary_operator.instruction;
    return ostr;
  }
};

struct ABinary_Operator {
public:
  virtual ~ABinary_Operator() = default;
protected:
  std::string instruction;
  friend std::ostream &operator<<(std::ostream &ostr, const ABinary_Operator& binary_operator) {
    ostr << binary_operator.instruction;
    return ostr;
  }
};

struct AIdentifier {
  std::string name;
  AIdentifier(std::string &name_) : name(name_) {}
  friend std::ostream &operator<<(std::ostream &ostr,
                                   const Identifier &identifier);
};

struct AAdd : ABinary_Operator {
  AAdd() { instruction = "addl"; }
};

struct ASub : ABinary_Operator {
  ASub() { instruction = "subl"; }
};

struct AMult : ABinary_Operator {
  AMult() { instruction = "imull"; }
};

struct Imm : Operand {
public:
  int val;
  Imm(int val_) : val(val_) {}

protected:
  void write(std::ostream &ostr) const override { ostr << "$" << val; }
};

struct AX : RegType {
  AX() { name = "%eax"; }
};

struct R10 : RegType {
  R10() { name = "%r10d"; }
};

struct DX: RegType {
  DX() { name = "%edx"; }
};

struct R11: RegType {
  R11() { name = "%r11d"; }
};

struct Reg : Operand {
public:
  RegType* reg;
  Reg(RegType* reg_): reg(reg_) {}
  ~Reg() { delete reg; }
protected:
  void write(std::ostream &ostr) const override { 
    ostr << *reg;
  }
};

struct Pseudo : Operand {
public:
  AIdentifier* identifier;
  Pseudo(AIdentifier* identifier_): identifier(identifier_){}
  ~Pseudo() { delete identifier; }
protected:
  void write(std::ostream &ostr) const override {
    ostr << ""; // Preventing Errors when compiling since this isn't being read as an assembly line
  };
};

struct Stack : Operand {
public:
  int offset;
  Stack(int offset_): offset(offset_){}
protected:
  void write(std::ostream &ostr) const override {
    ostr << offset << "(%rbp)";
  }
};

struct Mov : AInstruction {
public:
  Operand *src, *dst;
  Mov(Operand *src_, Operand *dst_) : src(src_), dst(dst_) {}
  ~Mov() {
    delete src;
    delete dst;
  }

protected:
  void write(std::ostream &ostr) const override {
    ostr << "movl" << "\t" << *src << ", " << *dst << std::endl;
  }
};

struct Neg: AUnary_Operator{
  Neg() { instruction = "negl"; }
};
struct Not: AUnary_Operator{
  Not() { instruction = "notl"; }
};

struct AUnary : AInstruction {
public:
   AUnary_Operator *unary_operator;
   Operand* operand;
   AUnary(AUnary_Operator *unary_operator_, Operand* operand_): unary_operator(unary_operator_), operand(operand_){}
   ~AUnary() {
    delete unary_operator;
    delete operand;
   }
protected:
  void write(std::ostream &ostr) const override {
    ostr << *unary_operator << "\t" << *operand << std::endl;
  }
};

struct ABinary : AInstruction {
public:
  ABinary_Operator *binary_operator;
  Operand *operand1, *operand2;
  ABinary(ABinary_Operator *binary_operator_, Operand *operand1_, Operand *operand2_): binary_operator(binary_operator_), operand1(operand1_), operand2(operand2_) {}
  ~ABinary() {
    delete binary_operator;
    delete operand1;
    delete operand2;
  }
protected:
  void write(std::ostream &ostr) const override {
    ostr << *binary_operator << "\t" << *operand1 << ", " << *operand2 << std::endl;
  }
};

struct AIdiv : AInstruction {
public:
  Operand *operand;
  AIdiv(Operand *operand_): operand(operand_) {}
  ~AIdiv() { delete operand; }
protected:
  void write(std::ostream &ostr) const override {
    ostr << "idivl" << "\t" << *operand << std::endl;
  }
};

struct ACdq : AInstruction {
protected:
  void write(std::ostream &ostr) const override {
    ostr << "cdq" << std::endl;
  }
};

struct AllocateStack : AInstruction {
public:
   int bytes;
   AllocateStack(int bytes_): bytes(bytes_){}
protected:
  void write(std::ostream& ostr) const override {
    ostr << "subq\t$" << bytes << ", %rsp" << std::endl;
  }
};

struct Ret : AInstruction {
public:
  std::string name;
  Ret() { name = "ret"; }

protected:
  void write(std::ostream &ostr) const override { 
    ostr << "\tmovq\t%rbp, %rsp" << std::endl;
    ostr << "\tpopq\t%rbp" << std::endl;
    ostr << "\t" << name << std::endl; 
  }
};

struct AFunction {
  AIdentifier *name;
  std::list<AInstruction *> instructions;
  AFunction(AIdentifier *name_, std::list<AInstruction *> &instructions_)
      : name(name_), instructions(instructions_) {}
  ~AFunction() {
    delete name;
    for (AInstruction *instr : instructions) {
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
