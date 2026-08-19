#include "aast/aast.hpp"
#include <ostream>
#include <memory>
#include <variant>

namespace aast
{
  std::ostream &operator<<(std::ostream &ostr, const Program &program)
  {
    for(auto &top_level : program.top_levels)
    {
      // visit allows us to call the same function for each type in the variant
      std::visit([&ostr](auto &val) {
        ostr << val << "\n";
      }, top_level);
    }
    ostr << ".section .note.GNU-stack,\"\",@progbits" << std::endl; // remove this for macos
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Static_Variable &static_variable)
  {
    if(static_variable.global) ostr << "\t" << ".globl " << *static_variable.name << "\n";
    ostr << "\t" << (static_variable.init == 0 ? ".bss" : ".data") << "\n";
    ostr << "\t" << ".balign 4" << "\n";
    ostr << *static_variable.name << ":\n"; // add an underscore before the name for macos
    ostr << "\t" << (static_variable.init == 0 ? ".zero 4" : ".long " + std::to_string(static_variable.init)) << "\n";
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Identifier &identifier)
  {
    ostr << identifier.text;
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Function &function)
  {
    // Add underscore before function name for macos
    if(function.global) ostr << "\t" << ".globl " << *function.name << "\n";
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
