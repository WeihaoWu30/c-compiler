#pragma once
#include <vector>
#include "ast/abstract/abstract.hpp"
#include "ast/operators/operators.hpp"
#include "ast/top_level/top_level.hpp"
#include "tacky/abstract/abstract.hpp"
#include "tacky/operators/operators.hpp"
#include "tacky/top_level/top_level.hpp"

namespace ir_gen
{
  extern uint32_t id_counter;
  extern uint32_t label_counter;
  tacky::Identifier *make_identifier();
  tacky::Unary_Operator convert_unop(ast::Unary_Operator op);
  tacky::Binary_Operator convert_binop(ast::Binary_Operator op);
  tacky::Val *emit_tacky(ast::Expression *e, std::vector<std::unique_ptr<tacky::Instruction>> &instructions, std::vector<std::unique_ptr<tacky::Val>> &values);
  tacky::Function *generate_function(ast::Function *func);
  tacky::Program *generate_tacky(ast::Program *program);
}
