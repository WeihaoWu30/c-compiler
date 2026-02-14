#include "aast.h"
#include "ast.h"
#include <iostream>
#include <vector>

AProgram *codegen(Program *&ast_program) {
  // disassemble
  Function *function = ast_program->func;
  Identifier *name = function->name;
  Return *statement = dynamic_cast<Return *>(function->body);
  if (!statement) {
    std::cerr << "Failed Converting Statement to Return" << std::endl;
    exit(1);
  }

  Constant *exp = dynamic_cast<Constant *>(statement->exp);
  if (!exp) {
    std::cerr << "Failed Converting Expression to Constant" << std::endl;
    exit(1);
  }

  // assemble
  Imm *constant = new Imm(exp->val);
  Register *reg = new Register();
  Mov *move = new Mov(constant, reg);
  Ret *ret = new Ret();
  std::vector<Instruction *> instructions;
  instructions.push_back(move);
  instructions.push_back(ret);

  AIdentifier *identifier = new AIdentifier(name->name);
  AFunction *aFunction = new AFunction(identifier, instructions);
  AProgram *program = new AProgram(aFunction);

  return program;
}
