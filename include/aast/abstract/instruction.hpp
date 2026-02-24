#pragma once
#include <ostream>

namespace aast
{
  struct Instruction
  {
  public:
    virtual ~Instruction() = default;

  protected:
    virtual void write(std::ostream &ostr) const = 0;
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Instruction &opr)
    {
      opr.write(ostr);
      return ostr;
    }
  };
}