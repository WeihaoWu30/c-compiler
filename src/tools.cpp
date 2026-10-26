#include "compiler/tools.hpp"
#include "ast/abstract/type.hpp"
#include "ast/types/types.hpp"
#include "ast/identifier_attrs/identifier_attr.hpp"
#include "aast/registers/reg_type.hpp"
#include <unordered_map>
#include <variant>
#include <string>
#include <cstdint>

namespace tools {
  std::unordered_map<std::string, std::pair<std::shared_ptr<ast::Type>, ast::Identifier_Attr>> frontend_symbols; // maps variable names to types
  std::unordered_map<std::string, asm_symtab_entry> backend_symbols; // maps assembly names to certain information about the variable or function
  uint32_t var_counter = 0;

  void generate_backend_from_frontend() 
  {
    for (auto& [name, entry] : tools::frontend_symbols) {
      auto& [type, attr] = entry;
      aast::Size size = aast::Size::DWORD;
      if(dynamic_cast<ast::Long*>(type.get())) {
        size = aast::Size::QWORD;
      }
      if (std::holds_alternative<ast::Fun_Attr>(attr)) {
        FunEntry function_entry(std::get<ast::Fun_Attr>(attr).defined);
        backend_symbols.insert_or_assign(name, function_entry); // cheap struct so we can copy
      } else {
        bool is_static = std::holds_alternative<ast::Static_Attr>(attr);
        ObjEntry object_entry(size, is_static);
        backend_symbols.insert_or_assign(name, object_entry); // cheap struct so we can copy
      }
    }
  }
} // namespace tools