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
    // add an underscore before the name for macos
#ifdef __APPLE__
    void write(std::ostream& ostr) const override { ostr << "_" << name->text << "(%rip)"; }
#else
    void write(std::ostream& ostr) const override { ostr << name->text << "(%rip)"; }
#endif
  };
} // namespace aast