#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include "aast/top_level/identifier.hpp"
#include <list>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

namespace aast {
  struct Function {
    std::list<std::unique_ptr<Instruction>> instructions;
    std::vector<std::unique_ptr<Operand>> operands;
    std::unique_ptr<Identifier> name;
    bool global;
    Function(std::unique_ptr<Identifier> name_, std::list<std::unique_ptr<Instruction>> instructions_, std::vector<std::unique_ptr<Operand>> operands_, bool global_)
        : instructions(std::move(instructions_)), operands(std::move(operands_)), name(std::move(name_)), global(global_) {}
    friend std::ostream& operator<<(std::ostream& ostr, const Function& function);
  };
} // namespace aast