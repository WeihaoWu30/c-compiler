#pragma once
#include "aast/abstract/reg_type.hpp"

namespace aast
{
  struct AX : RegType
  {
    AX() { name = "%eax"; }
  };

  struct R10 : RegType
  {
    R10() { name = "%r10d"; }
  };

  struct DX : RegType
  {
    DX() { name = "%edx"; }
  };

  struct R11 : RegType
  {
    R11() { name = "%r11d"; }
  };
}