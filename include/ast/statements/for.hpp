#pragma once
#include "ast/abstract/abstract.hpp"
#include "ast/top_level/identifier.hpp"

namespace ast
{
  struct For : Statement
  {
    For_Init *init;
    Expression *condition;
    Expression *post;
    Statement *body;
    Identifier *label;
    For(For_Init *init_, Statement *body_, Identifier *label_, Expression *condition_ = nullptr, Expression *post_ = nullptr) : init(init_), condition(condition_), post(post_), body(body_), label(label_) {}
    ~For() {
      delete init;
      delete body;
      delete label;
    }
  };
}