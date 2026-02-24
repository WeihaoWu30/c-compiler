#pragma once
#include <ostream>

namespace aast
{
  struct Operand
  {
  public:
    virtual ~Operand() = default;

  protected:
    virtual void write(std::ostream &ostr) const = 0;
    friend std::ostream &operator<<(std::ostream &ostr, const Operand &opr)
    {
      opr.write(ostr);
      return ostr;
    }
  };
}