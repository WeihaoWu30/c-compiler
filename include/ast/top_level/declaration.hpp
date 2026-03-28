#pragma once
#include "ast/abstract/expression.hpp"
#include "ast/top_level/identifier.hpp"
#include <ostream>
#include <memory>
#include <utility>

namespace ast
{
   struct Declaration
   {
      std::shared_ptr<Identifier> name;
      std::unique_ptr<Expression> init;
      Declaration(std::shared_ptr<Identifier> name_, std::unique_ptr<Expression> init_ = NULL) : name(std::move(name_)), init(std::move(init_)) {};
      friend std::ostream &operator<<(std::ostream &ostr, const Declaration &declaration);
   };
}