#pragma once
#include <string>
#include <ostream>

namespace aast
{
  struct Unary_Operator
  {
  public:
    virtual ~Unary_Operator() = default;

  protected:
    std::string instruction;
    friend std::ostream &operator<<(std::ostream &ostr, const Unary_Operator &unary_operator)
    {
      ostr << unary_operator.instruction;
      return ostr;
    }
  };
}