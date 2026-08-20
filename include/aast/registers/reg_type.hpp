#pragma once
#include <string>

namespace aast {
  enum class RegType { AX, CX, DX, DI, SI, R8, R9, R10, R11 };

  enum class Size { BYTE, DWORD, QWORD };

  inline std::string assembly_match(RegType regtype_, Size size_) {
    switch (regtype_) {
    case RegType::AX:
      switch (size_) {
      case Size::BYTE: return "%al";
      case Size::DWORD: return "%eax";
      case Size::QWORD: return "%rax";
      }
      break;
    case RegType::DX:
      switch (size_) {
      case Size::BYTE: return "%dl";
      case Size::DWORD: return "%edx";
      case Size::QWORD: return "%rdx";
      }
      break;
    case RegType::CX:
      switch (size_) {
      case Size::BYTE: return "%cl";
      case Size::DWORD: return "%ecx";
      case Size::QWORD: return "%rcx";
      }
      break;
    case RegType::DI:
      switch (size_) {
      case Size::BYTE: return "%dil";
      case Size::DWORD: return "%edi";
      case Size::QWORD: return "%rdi";
      }
      break;
    case RegType::SI:
      switch (size_) {
      case Size::BYTE: return "%sil";
      case Size::DWORD: return "%esi";
      case Size::QWORD: return "%rsi";
      }
      break;
    case RegType::R8:
      switch (size_) {
      case Size::BYTE: return "%r8b";
      case Size::DWORD: return "%r8d";
      case Size::QWORD: return "%r8";
      }
      break;
    case RegType::R9:
      switch (size_) {
      case Size::BYTE: return "%r9b";
      case Size::DWORD: return "%r9d";
      case Size::QWORD: return "%r9";
      }
      break;
    case RegType::R10:
      switch (size_) {
      case Size::BYTE: return "%r10b";
      case Size::DWORD: return "%r10d";
      case Size::QWORD: return "%r10";
      }
      break;
    case RegType::R11:
      switch (size_) {
      case Size::BYTE: return "%r11b";
      case Size::DWORD: return "%r11d";
      case Size::QWORD: return "%r11";
      }
      break;
    }
    return std::string();
  }
} // namespace aast