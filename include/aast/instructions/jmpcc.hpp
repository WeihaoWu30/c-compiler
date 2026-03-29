#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/top_level/identifier.hpp"
#include "aast/abstract/cond_code.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct JmpCC : Instruction
  {
  public:
    std::unique_ptr<Cond_Code> cond_code;
    std::shared_ptr<Identifier> identifier;
    JmpCC(std::unique_ptr<Cond_Code> cond_code_, std::shared_ptr<Identifier> identifier_) : cond_code(std::move(cond_code_)), identifier(std::move(identifier_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "j" << *cond_code << "\tL" << *identifier << std::endl;
    }
  };
}