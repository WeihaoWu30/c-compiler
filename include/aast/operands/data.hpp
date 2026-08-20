#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/top_level/identifier.hpp"
namespace aast {
  struct Data : Operand {
  public:
    Identifier* name;
    Data(Identifier* name_) : name(name_) {}
    ~Data() { delete name; }

  protected:
    void write(std::ostream& ostr) const override { ostr << name->text << "(%rip)"; }
  };
} // namespace aast