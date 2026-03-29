#pragma once
#include <vector>
#include "ast/abstract/abstract.hpp"
#include "ast/top_level/top_level.hpp"
#include "tacky/abstract/abstract.hpp"
#include "tacky/top_level/top_level.hpp"
#include <memory>

namespace ir_gen
{
  extern uint32_t id_counter;
  extern uint32_t label_counter;
  std::shared_ptr<tacky::Identifier> make_identifier();
  std::unique_ptr<tacky::Unary_Operator> convert_unop(std::unique_ptr<ast::Unary_Operator> exp);
  std::unique_ptr<tacky::Binary_Operator> convert_binop(std::unique_ptr<ast::Binary_Operator> op);
  std::shared_ptr<tacky::Val> emit_tacky(std::unique_ptr<ast::Expression> e, std::vector<std::unique_ptr<tacky::Instruction>> &instructions);
  std::unique_ptr<tacky::Function> generate_function(std::unique_ptr<ast::Function> func);
  std::unique_ptr<tacky::Program> generate_tacky(std::unique_ptr<ast::Program> program);
}
