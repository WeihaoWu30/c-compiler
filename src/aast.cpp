#include "aast/aast.hpp"
#include <memory>
#include <ostream>
#include <variant>

namespace aast {
  std::ostream& operator<<(std::ostream& ostr, const Program& program) {
    for (auto& top_level : program.top_levels) {
      // visit allows us to call the same function for each type in the variant
      std::visit([&ostr](auto& val) { ostr << val << "\n"; }, top_level);
    }
#ifndef __APPLE__
    ostr << ".section .note.GNU-stack,\"\",@progbits" << std::endl; // remove this for macos
#endif
    return ostr;
  }

  std::ostream& operator<<(std::ostream& ostr, const Static_Variable& static_variable) {
    if (static_variable.global) ostr << "\t" << ".globl ";
    bool is_bss = false;
    std::visit([&is_bss](auto& val) {
      if(val.value == 0) is_bss = true;
    }, static_variable.init);
#ifdef __APPLE__
    ostr << "_";
#endif
    ostr << *static_variable.name << "\n";
    ostr << "\t" << (is_bss ? ".bss" : ".data") << "\n";
    ostr << "\t"
         << ".balign "
         << static_variable.alignment
         << "\n";
#ifdef __APPLE__
    ostr << "_"; // add an underscore before the name for macos
#endif
    ostr << *static_variable.name << ":\n";
    ostr << "\t";

    if (std::holds_alternative<ast::IntInit>(static_variable.init)) {
      if(is_bss) {
        ostr << ".zero 4";
      } else {
        ostr << ".long " << std::get<ast::IntInit>(static_variable.init).value;
      }
    } else {
      if(is_bss) {
        ostr << ".zero 8";
      } else {
        ostr << ".quad " << std::get<ast::LongInit>(static_variable.init).value;
      }
    }
    
    ostr << "\n";
    return ostr;
  }

  std::ostream& operator<<(std::ostream& ostr, const Identifier& identifier) {
    ostr << identifier.text;
    return ostr;
  }

  std::ostream& operator<<(std::ostream& ostr, const Function& function) {
    // Add underscore before function name for macos
    if (function.global)
      ostr << "\t" << ".globl ";
#ifdef __APPLE__
    ostr << "_";;
#endif
    ostr << *function.name << ":\n";

    ostr << "\t"
         << "pushq\t%rbp\n";
    ostr << "\t"
         << "movq\t%rsp, %rbp\n";

    // Ret *return_instruction = nullptr; // final return instruction has to come after popping off the stack frame
    for (const std::unique_ptr<Instruction>& instr : function.instructions) {
      /*
      Ret *try_return = dynamic_cast<Ret *>(instr.get());
      if (try_return)
        return_instruction = try_return; */

      if (dynamic_cast<Label*>(instr.get())) {
        ostr << "\n" << *instr;
      } else if (dynamic_cast<Ret*>(instr.get())) { // emit inline Ret
        ostr << *instr;
      } else {
        ostr << "\t" << *instr;
      }
    }
    // ostr << *return_instruction << std::endl;
    return ostr;
  }

} // namespace aast
