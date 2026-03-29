#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/binary_operator.hpp"
#include "tacky/abstract/val.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Binary : Instruction
  {
    std::unique_ptr<Binary_Operator> binary_operator;
    std::shared_ptr<Val> src1, src2, dst;
    Binary(std::unique_ptr<Binary_Operator> binary_operator_,
           std::shared_ptr<Val> src1_, std::shared_ptr<Val> src2_,
           std::shared_ptr<Val> dst_) : binary_operator(std::move(binary_operator_)), src1(std::move(src1_)), src2(std::move(src2_)), dst(std::move(dst_)) {}
  };
}