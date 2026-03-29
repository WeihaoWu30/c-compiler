#pragma once
#include "aast/abstract/instruction.hpp"
#include "aast/abstract/operand.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace aast
{
  struct Mov : Instruction
  {
  public:
    std::shared_ptr<Operand> src, dst;
    Mov(std::shared_ptr<Operand> src_, std::shared_ptr<Operand> dst_) : src(std::move(src_)), dst(std::move(dst_)) {}

  protected:
    void write(std::ostream &ostr) const override
    {
      ostr << "movl" << "\t" << *src << ", " << *dst << std::endl;
    }
  };
}