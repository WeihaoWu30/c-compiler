#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky
{
  struct Copy : Instruction
  {
    Val *src, *dst;
    Copy(Val *src_, Val *dst_) : src(src_), dst(dst_) {}
  };
}