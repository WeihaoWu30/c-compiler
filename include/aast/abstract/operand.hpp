#pragma once
#include <ostream>
#include "aast/registers/reg_type.hpp"

namespace aast {
  struct Operand {
  public:
    Size size;
    virtual ~Operand() = default;

  protected:
    virtual void write(std::ostream& ostr) const = 0;
    friend std::ostream& operator<<(std::ostream& ostr, const Operand& opr) {
      opr.write(ostr);
      return ostr;
    }
  };
} // namespace aast