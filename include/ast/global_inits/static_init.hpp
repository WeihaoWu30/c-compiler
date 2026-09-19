#pragma once
#include <variant>

namespace ast {
  struct IntInit {
    int value;
    IntInit(int value_) : value(value_) {}
  };

  struct LongInit {
    long value;
    LongInit(long value_) : value(value_) {}
  };

  using StaticInit = std::variant<IntInit, LongInit>;
}