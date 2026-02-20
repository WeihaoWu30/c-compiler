#include <iostream>
#include "ast.h"

struct Program;
struct Function;
struct Identifier;

std::ostream &operator<<(std::ostream &ostr, const Program &program) {
  ostr << "Program(" << std::endl;
  ostr << "\t" << *(program.func); 
  ostr << ")" << std::endl;
  return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const Function &function) {
  ostr << "Function(" << std::endl;
  ostr << "\t\tname=" << *(function.name) << ", " << std::endl;
  ostr << "\t\tbody=" << *(function.body) << std::endl;
  ostr << "\t)" << std::endl;
  return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const Identifier &identifier) {
  ostr << "\"" << identifier.name << "\"";
  return ostr;
}

