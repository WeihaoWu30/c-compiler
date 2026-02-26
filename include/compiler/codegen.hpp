#pragma once
#include "aast/abstract/abstract.hpp"
#include "aast/operators/operators.hpp"
#include "aast/operands/pseudo.hpp"
#include "aast/operands/stack.hpp"
#include "aast/instructions/instructions.hpp"
#include "tacky/abstract/abstract.hpp"
#include "tacky/top_level/top_level.hpp"
#include <unordered_map>
#include <list>

namespace codegen
{
  extern std::unordered_map<std::string, int> stack_offset;
  extern uint32_t total_bytes_to_reserve;

  aast::Operand *generate_operand(tacky::Val *&t_val);
  aast::Unary_Operator *generate_unary_operators(tacky::Unary_Operator *unary_operator);
  aast::Binary_Operator *generate_basic_binary_operators(tacky::Binary_Operator *binary_operator);
  aast::Cond_Code *generate_conditional_codes(tacky::Binary_Operator *binary_operator);
  void generate_return(tacky::Instruction *&instruction, std::list<aast::Instruction *> &assembly_instructions);
  void generate_unary(tacky::Instruction *&instruction, std::list<aast::Instruction *> &assembly_instructions);
  void generate_bin(tacky::Instruction *&instruction, std::list<aast::Instruction *> &assembly_instructions);
  void generate_jmp_if(tacky::Instruction *&instruction, std::list<aast::Instruction *> &assembly_instructions);
  void generate_jmp(tacky::Instruction *&instruction, std::list<aast::Instruction *> &assembly_instructions);
  void generate_label(tacky::Instruction *&instruction, std::list<aast::Instruction *> &assembly_instructions);
  void generate_copy(tacky::Instruction *&instruction, std::list<aast::Instruction *> &assembly_instructions);
  std::list<aast::Instruction *> generate_instructions(tacky::Function *func);
  aast::Stack *replace_pseudo(aast::Pseudo *pseudo);
  void fix_mov(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_unary(typename std::list<aast::Instruction *>::iterator &it);
  void fix_add_sub(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_mult(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_div(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_cmp(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions);
  void fix_set(typename std::list<aast::Instruction *>::iterator &it);
  void compiler_pass(std::list<aast::Instruction *> &instructions);
  aast::Program *generate_top_level(tacky::Program *&tacky_program);
}
