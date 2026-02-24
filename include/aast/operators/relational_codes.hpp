#pragma once
#include "aast/abstract/cond_code.hpp"

namespace aast
{
  struct E : Cond_Code
  {
    E() { code = "e"; }
  };

  struct NE : Cond_Code
  {
    NE() { code = "ne"; }
  };

  struct G : Cond_Code
  {
    G() { code = "g"; }
  };

  struct GE : Cond_Code
  {
    GE() { code = "ge"; }
  };

  struct L : Cond_Code
  {
    L() { code = "l"; }
  };

  struct LE : Cond_Code
  {
    LE() { code = "le"; }
  };
}