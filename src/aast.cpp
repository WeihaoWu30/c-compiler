#include "aast/aast.hpp"
#include <ostream>
#include <memory>

namespace aast
{
  std::ostream &operator<<(std::ostream &ostr, const Program &program)
  {
    for(auto &function_definition : program.function_definitions)
    {
      ostr << *function_definition << "\n";
    }
    ostr << ".section .note.GNU-stack,\"\",@progbits" << std::endl; // remove this for macos
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Identifier &identifier)
  {
    ostr << identifier.name;
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Function &function)
  {
    // Add underscore before function name for macos
    ostr << "\t" << ".globl " << *function.name << "\n";
    ostr << *function.name << ":\n";
    ostr << "\t" << "pushq\t%rbp\n";
    ostr << "\t" << "movq\t%rsp, %rbp\n";

    // Ret *return_instruction = nullptr; // final return instruction has to come after popping off the stack frame
    for (const std::unique_ptr<Instruction> &instr : function.instructions)
    {
      /*
      Ret *try_return = dynamic_cast<Ret *>(instr.get());
      if (try_return)
        return_instruction = try_return; */

      if (dynamic_cast<Label *>(instr.get()))
      {
        ostr << "\n"
             << *instr;
      }
      else if (dynamic_cast<Ret *>(instr.get()))
      { // emit inline Ret
        ostr << *instr;
      }
      else
      {
        ostr << "\t" << *instr;
      }
    }
    // ostr << *return_instruction << std::endl;
    return ostr;
  }

}
