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
  for (Instruction *instr : function.instructions) {
    ostr << "\t" << *instr << std::endl;
  }
  return ostr;
}

