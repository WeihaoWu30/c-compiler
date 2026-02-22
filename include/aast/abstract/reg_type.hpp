#pragma once
#include <string>
#include <ostream>

namespace aast
{
  struct RegType
  {
  public:
    virtual ~RegType() = default;

  protected:
    std::string name;
    friend std::ostream &operator<<(std::ostream &ostr, const RegType &regType)
    {
      ostr << regType.name;
      return ostr;
    }
  };
}