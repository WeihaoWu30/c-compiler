#pragma once
#include "aast/abstract/operand.hpp"
#include "aast/top_level/identifier.hpp"
#include "aast/registers/reg_type.hpp"
#include <ostream>

namespace aast {
  struct Pseudo : Operand {
  public:
    Identifier* identifier; 
    Pseudo(Identifier* identifier_, Size size_) : identifier(identifier_) {
      size = size_;
    }
    ~Pseudo() { delete identifier; }

  protected:
    void write(std::ostream& ostr) const override {
      ostr << ""; // Preventing Errors when compiling since this isn't being read as an assembly line
    };
  };
} // namespace aast