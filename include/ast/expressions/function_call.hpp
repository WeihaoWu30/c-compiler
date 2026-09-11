#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"
#include <ast/abstract/type.hpp>
#include <vector>
#include <memory>

namespace ast {
  struct Function_Call : Expression {
    Identifier* identifier;
    std::vector<Expression*> args;
    Function_Call(Identifier* identifier_, std::vector<Expression*>& args_, std::shared_ptr<Type> type_ = nullptr) : identifier(identifier_), args(args_) {
      type = type_;
    }
    ~Function_Call() { delete identifier; }
  };
} // namespace ast