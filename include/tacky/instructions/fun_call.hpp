#pragma once
#include "tacky/abstract/instruction.hpp"
#include "tacky/abstract/val.hpp"
#include "tacky/top_level/identifier.hpp"
#include <vector>

namespace tacky {
  struct Fun_Call : Instruction {
    Identifier* fun_name;
    std::vector<Val*> args;
    Val* dst;
    Fun_Call(Identifier* fun_name_, std::vector<Val*> args_, Val* dst_) : fun_name(fun_name_), args(args_), dst(dst_) {}
    ~Fun_Call() { delete fun_name; }
  };
} // namespace tacky