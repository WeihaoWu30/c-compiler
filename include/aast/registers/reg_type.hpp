#pragma once
#include <string>

namespace aast
{
  enum class RegType
  {
    AX,
    CX,
    DX,
    R10,
    R11
  };

  enum class Size
  {
    BYTE,
    DWORD
  };

  inline std::string assembly_match(RegType regtype_, Size size_)
  {
    switch (regtype_)
    {
    case RegType::AX:
      switch (size_)
      {
      case Size::BYTE:
        return "%al";
      case Size::DWORD:
        return "%eax";
      }
    case RegType::DX:
      switch (size_)
      {
      case Size::BYTE:
        return "%dl";
      case Size::DWORD:
        return "%edx";
      }
    case RegType::R10:
      switch (size_)
      {
      case Size::BYTE:
        return "%r10b";
      case Size::DWORD:
        return "%r10d";
      }
    case RegType::R11:
      switch (size_)
      {
      case Size::BYTE:
        return "%r11b";
      case Size::DWORD:
        return "%r11d";
      }
    case RegType::CX:
      switch (size_)
      {
      case Size::BYTE:
        return "%cl";
      case Size::DWORD:
        return "%ecx";
      }
    }
  }
}