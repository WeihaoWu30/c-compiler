#pragma once
#include "aast/abstract/abstract.hpp"
#include "aast/operators/operators.hpp"
#include "aast/operands/pseudo.hpp"
#include "aast/operands/stack.hpp"
#include "aast/instructions/instructions.hpp"
#include "tacky/abstract/abstract.hpp"
#include "tacky/operators/operators.hpp"
#include "tacky/top_level/top_level.hpp"
#include <unordered_map>
#include <list>
#include <vector>
#include <memory>
#include <array>

namespace codegen
{
  extern std::unordered_map<std::string, int> stack_offset;
  extern std::unordered_map<int, aast::Stack *> stack_objs;
  constexpr std::array<tacky::Binary_Operator, 8> basic_operators = {
      tacky::Binary_Operator::Add,
      tacky::Binary_Operator::Subtract,
      tacky::Binary_Operator::Multiply,
      tacky::Binary_Operator::BitAnd,
      tacky::Binary_Operator::BitOr,
      tacky::Binary_Operator::BitXor,
      tacky::Binary_Operator::BitRightShift,
      tacky::Binary_Operator::BitLeftShift,
  };

  constexpr std::array<tacky::Binary_Operator, 2> complex_operators = {
      tacky::Binary_Operator::Remainder,
      tacky::Binary_Operator::Divide,
  };

  constexpr std::array<tacky::Binary_Operator, 6> logical_operators = {
      tacky::Binary_Operator::Equal,
      tacky::Binary_Operator::NotEqual,
      tacky::Binary_Operator::LessThan,
      tacky::Binary_Operator::LessOrEqual,
      tacky::Binary_Operator::GreaterThan,
      tacky::Binary_Operator::GreaterOrEqual,
  };

  extern uint32_t total_bytes_to_reserve;

  aast::Operand *generate_operand(tacky::Val *t_val, std::vector<std::unique_ptr<aast::Operand>> &operands);
  aast::Unary_Operator *generate_unary_operators(tacky::Unary_Operator unary_operator);
  aast::Binary_Operator *generate_basic_binary_operators(tacky::Binary_Operator binary_operator);
  aast::Cond_Code *generate_conditional_codes(tacky::Binary_Operator binary_operator);
  void generate_return(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void generate_unary(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void generate_bin(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void generate_jmp_if(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void generate_jmp(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions);
  void generate_label(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions);
  void generate_copy(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void generate_instructions(tacky::Function *func, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  aast::Stack *replace_pseudo(aast::Pseudo *pseudo, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void fix_mov(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void fix_unary(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void fix_add_sub(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void fix_mult(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void fix_div(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void fix_cmp(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void fix_set(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::vector<std::unique_ptr<aast::Operand>> &operands);
  void compiler_pass(std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands);
  aast::Program *generate_top_level(tacky::Program *&tacky_program);
}
