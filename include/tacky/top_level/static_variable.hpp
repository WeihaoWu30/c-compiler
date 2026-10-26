#pragma once
#include "ast/global_inits/static_init.hpp"
#include "tacky/top_level/identifier.hpp"
#include "ast/global_inits/global_inits.hpp"
#include "ast/abstract/type.hpp"
#include <memory>
#include <utility>

namespace tacky {
  struct Static_Variable {
    std::unique_ptr<Identifier> identifier;
    std::shared_ptr<ast::Type> type;
    ast::StaticInit init;
    bool global;
    Static_Variable(std::unique_ptr<Identifier> identifier_, std::shared_ptr<ast::Type> type_, ast::StaticInit init_, bool global_) : identifier(std::move(identifier_)), type(type_), init(std::move(init_)), global(global_) {}
  };
} // namespace tacky