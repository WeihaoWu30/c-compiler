#pragma once
#include "ast/abstract/type.hpp"
#include "ast/identifier_attrs/identifier_attr.hpp"
#include "aast/registers/reg_type.hpp"
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <variant>

namespace tools {
  extern std::unordered_map<std::string, std::pair<std::shared_ptr<ast::Type>, ast::Identifier_Attr>> frontend_symbols; // maps variable names to types

  struct ObjEntry {
    aast::Size size;
    bool is_static;
    ObjEntry(aast::Size size_, bool is_static_) : size(size_), is_static(is_static_) {}
  };
  struct FunEntry {
    bool defined;
    FunEntry(bool defined_) : defined(defined_) {}
  };
  using asm_symtab_entry = std::variant<ObjEntry, FunEntry>;

  extern std::unordered_map<std::string, asm_symtab_entry> backend_symbols; // maps assembly names to certain information about the variable or function
  extern uint32_t var_counter;
  static const std::regex naming_convention("^[a-zA-Z_][a-zA-Z0-9_]*$");
  void generate_backend_from_frontend();
} // namespace tools