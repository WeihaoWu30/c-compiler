#pragma once
#include "ast/abstract/for_init.hpp"
#include "ast/abstract/expression.hpp"

namespace ast
{
  struct Init_Exp : For_Init
  {
    Expression *expression;
    Init_Exp(Expression *expression_ = nullptr) : expression(expression_) {}
  };
}