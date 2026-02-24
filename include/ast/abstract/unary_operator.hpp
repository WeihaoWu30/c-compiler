#pragma once
#include <ostream>
#include <string>

namespace ast
{
  struct Unary_Operator
  {
  public:
    virtual ~Unary_Operator() = default;

  protected:
    std::string symbol;
    friend std::ostream &operator<<(std::ostream &ostr, const Unary_Operator &unary_operator)
    {
      ostr << "Unary Operator(" << unary_operator.symbol << ")";
      return ostr;
    }
  };
}
