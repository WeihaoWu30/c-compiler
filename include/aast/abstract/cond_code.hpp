#pragma once
#include <ostream>

namespace aast
{
  struct Cond_Code
  {
  public:
    virtual ~Cond_Code() = default;

  protected:
    std::string code;
    friend std::ostream &operator<<(std::ostream &ostr, const Cond_Code &cond_code)
    {
      ostr << cond_code.code;
      return ostr;
    }
  };
}