#include "aast/aast.hpp"
#include <ostream>

namespace aast
{
  std::ostream &operator<<(std::ostream &ostr, const Program &program)
  {
    ostr << *(program.function_definition) << std::endl;
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Identifier &identifier)
  {
    ostr << identifier.name;
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Function &function)
  {
    ostr << "\t" << ".globl _" << *function.name << std::endl;
    ostr << "_" << *function.name << ":" << std::endl;
    ostr << "\t" << "pushq\t%rbp" << std::endl;
    ostr << "\t" << "movq\t%rsp, %rbp" << std::endl;

    Ret *return_instruction = nullptr; // return instruction has to come after popping off the stack frame
    for (Instruction *instr : function.instructions)
    {
      return_instruction = dynamic_cast<Ret *>(instr);
      if (return_instruction)
        continue;
      ostr << "\t" << *instr;
    }

    ostr << *return_instruction << std::endl;
    return ostr;
  }

}
