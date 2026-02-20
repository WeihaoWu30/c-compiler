#include "aast.h"
#include <iostream>

struct AProgram;
struct AFunction;
struct AIdentifier;

std::ostream &operator<<(std::ostream &ostr, const AProgram &program) {
  ostr << *(program.function_definition) << std::endl;
  return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const AIdentifier &identifier) {
  ostr << identifier.name;
  return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const AFunction &function) {
  ostr << "\t" << ".globl _" << *function.name << std::endl;
  ostr << "_" << *function.name << ":" << std::endl;
  ostr << "\t" << "pushq\t%rbp" << std::endl;
  ostr << "\t" << "movq\t%rsp, %rbp" << std::endl;

  Ret *return_instruction; // return instruction has to come after popping off the stack frame
  for (Instruction *instr : function.instructions) {
    return_instruction = dynamic_cast<Ret *>(instr);
    if(return_instruction) continue;
    ostr << "\t" << *instr;
  }

  ostr << "\t" << *return_instruction << std::endl;
  return ostr;
}

