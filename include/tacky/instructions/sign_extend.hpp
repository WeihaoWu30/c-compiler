#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"

namespace tacky {
  struct SignExtend : Instruction {
    Val *src, *dst;
    SignExtend(Val *src_, Val *dst_) : src(src_), dst(dst_) {}
  };
}