#pragma once
#include "aast/abstract/abstract.hpp"
#include "aast/operators/operators.hpp"
#include "aast/operands/stack.hpp"
#include "tacky/abstract/abstract.hpp"
#include "tacky/top_level/top_level.hpp"
#include <unordered_map>

namespace codegen
{
  extern std::unordered_map<std::string, int> stack_offset;
  extern uint32_t total_bytes_to_reserve;

  aast::Operand *generate_operand(tacky::Val *&t_val);
  aast::Unary_Operator *generate_unary_operators(tacky::Unary_Operator *unary_operator);
  aast::Binary_Operator *generate_basic_binary_operators(tacky::Binary_Operator *binary_operator);
  void generate_return(tacky::Return *t_return, std::list<aast::Instruction *> &assembly_instructions);
  void generate_unary(tacky::Unary *t_unary, std::list<aast::Instruction *> &assembly_instructions);
  void generate_bin_basic(tacky::Binary *t_binary, std::list<aast::Instruction *> &assembly_instructions);
  void generate_bin_div(tacky::Binary *t_binary, std::list<aast::Instruction *> &assembly_instructions);
  std::list<aast::Instruction *> generate_instructions(tacky::Function *func);
  aast::Stack *replace_pseudo(aast::Pseudo *pseudo);
  void fix_mov(aast::Mov *&mov, typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_unary(aast::Unary *&unary);
  void fix_add_sub(aast::Binary *&binary, typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_mult(aast::Binary *&binary, typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_div(aast::Idiv *&idiv, typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void compiler_pass(std::list<aast::Instruction *> &instructions);
  aast::Program *generate_top_level(tacky::Program *&tacky_program);
}
