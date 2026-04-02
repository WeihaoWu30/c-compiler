#include "aast/aast.hpp"
#include <ostream>
#include <memory>

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

    std::unique_ptr<Ret> return_instruction = std::make_unique<Ret>(); // return instruction has to come after popping off the stack frame
    for (const std::unique_ptr<Instruction> &instr : function.instructions)
    {
      aast::Ret *ret = dynamic_cast<Ret *>(instr.get());
      if (ret)
        continue;

      if (dynamic_cast<Label *>(instr.get()))
      {
        ostr << std::endl
             << *instr;
      }
      else
      {
        ostr << "\t" << *instr;
      }
    }
    ostr << *return_instruction << std::endl;
    return ostr;
  }

}
