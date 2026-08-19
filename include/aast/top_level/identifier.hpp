#pragma once
#include <string>
#include <ostream>

namespace aast
{
  struct Identifier
  {
    std::string text;
    Identifier(std::string &text_) : text(text_) {}
    friend std::ostream &operator<<(std::ostream &ostr,
                                    const Identifier &identifier);
  };
}