#pragma once
#include <ostream>
#include <string>

namespace aast {
  struct Binary_Operator {
  public:
    virtual ~Binary_Operator() = default;

  protected:
    std::string instruction;
    friend std::ostream& operator<<(std::ostream& ostr, const Binary_Operator& binary_operator) {
      ostr << binary_operator.instruction;
      return ostr;
    }
  };
} // namespace aast