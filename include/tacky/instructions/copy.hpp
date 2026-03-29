#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"
#include <memory>
#include <utility>

namespace tacky
{
  struct Copy : Instruction
  {
    std::shared_ptr<Val> src, dst;
    Copy(std::shared_ptr<Val> src_, std::shared_ptr<Val> dst_) : src(std::move(src_)), dst(std::move(dst_)) {}
  };
}