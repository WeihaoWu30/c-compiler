#pragma once
#include <format>
#include <stdexcept>
#include <string>

namespace ast {
  enum class Storage_Class { STATIC, EXTERN, NONE };

  inline Storage_Class get_storage_class(const std::string& token) {
    if (token == "static") {
      return Storage_Class::STATIC;
    } else if (token == "extern") {
      return Storage_Class::EXTERN;
    } else {
      throw std::runtime_error(std::format("Invalid storage class: {}", token));
    }
  }
} // namespace ast