#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Reg : Operand
  {
  public:
    std::shared_ptr<RegType> reg;
    Reg(std::shared_ptr<RegType> reg_) : reg(std::move(reg_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << *reg;
    }
  };
}