#pragma once
#include "aast/top_level/identifier.hpp"
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <list>
#include <vector>
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Function
  {
    Identifier *name;
    std::list<std::unique_ptr<Instruction>> instructions;
    std::vector<std::unique_ptr<Operand>> operands;
    Function(Identifier *name_, std::list<std::unique_ptr<Instruction>> instructions_, std::vector<std::unique_ptr<Operand>> operands_)
        : name(name_), instructions(std::move(instructions_)), operands(std::move(operands_)) {}
    ~Function()
    {
      delete name;
    }
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Function &function);
  };
}