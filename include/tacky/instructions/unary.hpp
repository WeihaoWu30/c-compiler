#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/unary_operator.hpp"
#include "tacky/abstract/val.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Unary : Instruction
  {
    std::unique_ptr<Unary_Operator> unary_operator;
    std::shared_ptr<Val> src, dst;
    Unary(std::unique_ptr<Unary_Operator> unary_operator_, std::shared_ptr<Val> src_, std::shared_ptr<Val> dst_) : unary_operator(std::move(unary_operator_)), src(std::move(src_)), dst(std::move(dst_)) {}
  };
}