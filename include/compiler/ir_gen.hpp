#pragma once
#include <vector>
#include "ast/abstract/abstract.hpp"
#include "ast/top_level/top_level.hpp"
#include "tacky/abstract/abstract.hpp"
#include "tacky/top_level/top_level.hpp"

namespace ir_gen
{
  extern uint32_t counter;
  tacky::Identifier *make_temporary();
  tacky::Unary_Operator *convert_unop(ast::Unary_Operator *exp);
  tacky::Binary_Operator *convert_binop(ast::Binary_Operator *op);
  tacky::Val *emit_tacky(ast::Expression *e, std::vector<tacky::Instruction *> &instructions);
  tacky::Function *generate_function(ast::Function *func);
  tacky::Program *generate_tacky(ast::Program *program);
}
