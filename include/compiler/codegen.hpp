#pragma once
#include "aast/abstract/abstract.hpp"
#include "aast/instructions/instructions.hpp"
#include "aast/operands/pseudo.hpp"
#include "aast/operands/stack.hpp"
#include "aast/operators/operators.hpp"
#include "aast/top_level/program.hpp"
#include "tacky/abstract/abstract.hpp"
#include "tacky/abstract/val.hpp"
#include "tacky/operators/operators.hpp"
#include "tacky/top_level/top_level.hpp"
#include <array>
#include <cstddef>
#include <list>
#include <memory>
#include <vector>

namespace codegen {
  struct StackManager;
  constexpr std::array<aast::RegType, 6> argument_registers = {
      aast::RegType::DI, aast::RegType::SI, aast::RegType::DX, aast::RegType::CX, aast::RegType::R8, aast::RegType::R9,
  };
  constexpr std::array<tacky::Binary_Operator, 8> basic_operators = {
      tacky::Binary_Operator::Add,   tacky::Binary_Operator::Subtract, tacky::Binary_Operator::Multiply,      tacky::Binary_Operator::BitAnd,
      tacky::Binary_Operator::BitOr, tacky::Binary_Operator::BitXor,   tacky::Binary_Operator::BitRightShift, tacky::Binary_Operator::BitLeftShift,
  };

  constexpr std::array<tacky::Binary_Operator, 2> complex_operators = {
      tacky::Binary_Operator::Remainder,
      tacky::Binary_Operator::Divide,
  };

  constexpr std::array<tacky::Binary_Operator, 6> logical_operators = {
      tacky::Binary_Operator::Equal,       tacky::Binary_Operator::NotEqual,    tacky::Binary_Operator::LessThan,
      tacky::Binary_Operator::LessOrEqual, tacky::Binary_Operator::GreaterThan, tacky::Binary_Operator::GreaterOrEqual,
  };
  constexpr std::size_t MAX_PARAMS_IN_REGISTERS = 6;
  aast::Size get_size_based_on_name(const std::string& name);
  aast::Size get_size_based_on_val(tacky::Val* val);
  aast::Operand* generate_operand(tacky::Val* t_val, std::vector<std::unique_ptr<aast::Operand>>& operands);
  aast::Unary_Operator* generate_unary_operators(tacky::Unary_Operator unary_operator);
  aast::Binary_Operator* generate_basic_binary_operators(tacky::Binary_Operator binary_operator);
  aast::Cond_Code* generate_conditional_codes(tacky::Binary_Operator binary_operator);
  void generate_return(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_unary(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_bin(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_com(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_jmp_if(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_jmp(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions);
  void generate_label(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions);
  void generate_copy(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_sign_extend(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_truncate(tacky::Instruction* instruction, std::list<std::unique_ptr<aast::Instruction>>& assembly_instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void generate_instructions(std::vector<std::unique_ptr<tacky::Instruction>>& body, std::list<std::unique_ptr<aast::Instruction>>& instructions,
                             std::vector<std::unique_ptr<aast::Operand>>& operands);
  aast::Stack* replace_pseudo(aast::Pseudo* pseudo, std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void fix_mov(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions, std::vector<std::unique_ptr<aast::Operand>>& operands,
               StackManager& stack_manager);
  void fix_unary(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void fix_shifting(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions,
                    std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void fix_basic(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions,
                 std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void fix_mult(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions,
                std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void fix_div(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions, std::vector<std::unique_ptr<aast::Operand>>& operands,
               StackManager& stack_manager);
  void fix_cmp(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions, std::vector<std::unique_ptr<aast::Operand>>& operands,
               StackManager& stack_manager);
  void fix_set(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void fix_movsx(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions, std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void fix_push(typename std::list<std::unique_ptr<aast::Instruction>>::iterator& it, std::list<std::unique_ptr<aast::Instruction>>& instructions, std::vector<std::unique_ptr<aast::Operand>>& operands, StackManager& stack_manager);
  void copy_parameters(std::vector<std::unique_ptr<tacky::Identifier>>& params, std::list<std::unique_ptr<aast::Instruction>>& instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  void compiler_pass(StackManager& stack_manager, std::list<std::unique_ptr<aast::Instruction>>& instructions, std::vector<std::unique_ptr<aast::Operand>>& operands);
  aast::Function generate_function(tacky::Function* func);
  aast::Static_Variable generate_static_variable(tacky::Static_Variable* static_variable);
  aast::Program* generate_top_level(tacky::Program* tacky_program);
} // namespace codegen
