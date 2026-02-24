#pragma once
#include <string>
#include <ostream>

namespace aast
{
  struct Identifier
  {
    std::string name;
    Identifier(std::string &name_) : name(name_) {}
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Identifier &identifier);
  };
}