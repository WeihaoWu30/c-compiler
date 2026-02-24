#pragma once
#include <string>

namespace tacky
{
  struct Identifier
  {
    std::string name;
    Identifier(std::string name_) : name(name_) {}
  };
}