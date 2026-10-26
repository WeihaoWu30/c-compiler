#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky {
  struct Truncate : Instruction {
    Val *src, *dst;
    Truncate(Val *src_, Val *dst_) : src(src_), dst(dst_) {}
  };
}