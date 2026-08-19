#include "aast/aast.hpp"
#include "tacky/tacky.hpp"
#include "compiler/codegen.hpp"
#include "compiler/symbols.hpp"
#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <algorithm>
#include <cstddef>
#include <variant>

namespace codegen
{
  struct StackManager {
    std::unordered_map<std::string, int> stack_offset{}; // unique rbp jumps
    std::unordered_map<int, aast::Stack *> stack_objs{}; // copyable stack objects
    uint32_t total_bytes_to_reserve = 0; // total bytes to allocate to aast::Stack Frame
  };

  // This function converts Tacky Values to Immediate Values and Temporary Variables
  aast::Operand *generate_operand(tacky::Val *t_val, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Constant *t_constant = dynamic_cast<tacky::Constant *>(t_val);
    tacky::Var *t_var = dynamic_cast<tacky::Var *>(t_val);
    if (t_constant)
    {
      std::unique_ptr<aast::Imm> immediate_value = std::make_unique<aast::Imm>(t_constant->val);
      operands.push_back(std::move(immediate_value));
      return operands.back().get();
    }
    else if (t_var)
    {
      aast::Identifier *assembly_identifier = new aast::Identifier(t_var->identifier->name);
      if(auto it = symbols::symbols.find(t_var->identifier->name); it != symbols::symbols.end()) {
        if(std::holds_alternative<ast::Static_Attr>(it->second.second)) {
          std::unique_ptr<aast::Data> data = std::make_unique<aast::Data>(assembly_identifier);
          operands.push_back(std::move(data));
          return operands.back().get();
        }
      }
      std::unique_ptr<aast::Pseudo> pseudo_identifier = std::make_unique<aast::Pseudo>(assembly_identifier);
      operands.push_back(std::move(pseudo_identifier));
      return operands.back().get();
    } else {
      throw std::runtime_error("Tacky Value cannot be read as an operand.");
    }
    return nullptr;
  }

  // This function converts Tacky unary operators to assembly instructions for unary operators
  aast::Unary_Operator *generate_unary_operators(tacky::Unary_Operator unary_operator)
  {
    switch (unary_operator)
    {
    case tacky::Unary_Operator::Negate:
      return new aast::Neg();
    case tacky::Unary_Operator::Complement:
    case tacky::Unary_Operator::Not:
      return new aast::Not();
    default:
      return nullptr;
    }
  }

  // This function converts from TACKY to assembly Instructions for Binary Operators +, -, *
  aast::Binary_Operator *generate_basic_binary_operators(tacky::Binary_Operator binary_operator)
  {
    switch (binary_operator)
    {
    case tacky::Binary_Operator::Add:
      return new aast::Add();
    case tacky::Binary_Operator::Subtract:
      return new aast::Sub();
    case tacky::Binary_Operator::Multiply:
      return new aast::Mult();
    case tacky::Binary_Operator::BitAnd:
      return new aast::And();
    case tacky::Binary_Operator::BitOr:
      return new aast::Or();
    case tacky::Binary_Operator::BitXor:
      return new aast::Xor();
    case tacky::Binary_Operator::BitLeftShift:
      return new aast::Shl();
    case tacky::Binary_Operator::BitRightShift:
      return new aast::Sar();
    default:
      return nullptr;
    }
  }

  // This function converts the relational operators into the assembly code
  aast::Cond_Code *generate_conditional_codes(tacky::Binary_Operator binary_operator)
  {
    switch (binary_operator)
    {
    case tacky::Binary_Operator::Equal:
      return new aast::E();
    case tacky::Binary_Operator::NotEqual:
      return new aast::NE();
    case tacky::Binary_Operator::LessThan:
      return new aast::L();
    case tacky::Binary_Operator::LessOrEqual:
      return new aast::LE();
    case tacky::Binary_Operator::GreaterThan:
      return new aast::G();
    case tacky::Binary_Operator::GreaterOrEqual:
      return new aast::GE();
    default:
      return nullptr;
    }
  }

  // This function creates the assembly Return Instruction
  void generate_return(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Return *t_return = dynamic_cast<tacky::Return *>(instruction);
    if (!t_return)
      return;
    aast::Operand *val = generate_operand(t_return->val, operands);
    std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::AX, aast::Size::DWORD);
    std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(val, reg.get());
    assembly_instructions.push_back(std::move(mov));

    std::unique_ptr<aast::Ret> ret = std::make_unique<aast::Ret>();
    assembly_instructions.push_back(std::move(ret));

    operands.push_back(std::move(reg));
  }

  // This function creates the assembly Instruction for tacky Unary Operators
  void generate_unary(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Unary *t_unary = dynamic_cast<tacky::Unary *>(instruction);
    if (!t_unary)
      return;
    if (t_unary->unary_operator == tacky::Unary_Operator::Not)
    {
      aast::Operand *src = generate_operand(t_unary->src, operands);
      aast::Operand *dst = generate_operand(t_unary->dst, operands);
      std::unique_ptr<aast::Imm> imm = std::make_unique<aast::Imm>(0);
      std::unique_ptr<aast::Cmp> cmp = std::make_unique<aast::Cmp>(imm.get(), src);
      assembly_instructions.push_back(std::move(cmp));

      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(imm.get(), dst);
      assembly_instructions.push_back(std::move(mov));

      aast::E *eq = new aast::E();
      std::unique_ptr<aast::SetCC> set_cc = std::make_unique<aast::SetCC>(eq, dst);
      assembly_instructions.push_back(std::move(set_cc));

      operands.push_back(std::move(imm));
    }
    else
    {
      aast::Operand *src = generate_operand(t_unary->src, operands);
      aast::Operand *dst = generate_operand(t_unary->dst, operands);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(src, dst);
      assembly_instructions.push_back(std::move(mov));

      aast::Unary_Operator *unary_operator = generate_unary_operators(t_unary->unary_operator);
      std::unique_ptr<aast::Unary> unary = std::make_unique<aast::Unary>(unary_operator, dst);
      assembly_instructions.push_back(std::move(unary));
    }
  }

  // This function creates the assembly Instruction for tacky Binary Operators
  void generate_bin(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Binary *t_binary = dynamic_cast<tacky::Binary *>(instruction);
    if (!t_binary)
      return;
    if (std::find(basic_operators.begin(), basic_operators.end(), t_binary->binary_operator) != basic_operators.end())
    {
      aast::Operand *mov_src = generate_operand(t_binary->src1, operands);
      aast::Operand *dst = generate_operand(t_binary->dst, operands);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(mov_src, dst);
      assembly_instructions.push_back(std::move(mov));

      aast::Operand *bin_src = generate_operand(t_binary->src2, operands);
      aast::Binary_Operator *bin_op = generate_basic_binary_operators(t_binary->binary_operator);
      std::unique_ptr<aast::Binary> binary = std::make_unique<aast::Binary>(bin_op, bin_src, dst);
      assembly_instructions.push_back(std::move(binary));
    }
    else if (std::find(complex_operators.begin(), complex_operators.end(), t_binary->binary_operator) != complex_operators.end())
    {
      std::unique_ptr<aast::Reg> mov_reg1 = std::make_unique<aast::Reg>(aast::RegType::AX, aast::Size::DWORD);

      aast::Operand *mov_src = generate_operand(t_binary->src1, operands); // dividend
      std::unique_ptr<aast::Mov> mov1 = std::make_unique<aast::Mov>(mov_src, mov_reg1.get());
      assembly_instructions.push_back(std::move(mov1));

      std::unique_ptr<aast::Cdq> cdq = std::make_unique<aast::Cdq>(); // sign extension since idiv can only take in 6 bit values
      assembly_instructions.push_back(std::move(cdq));

      aast::Operand *div_src = generate_operand(t_binary->src2, operands); // divisor
      std::unique_ptr<aast::Idiv> idiv = std::make_unique<aast::Idiv>(div_src);
      assembly_instructions.push_back(std::move(idiv));

      std::unique_ptr<aast::Reg> mov_reg2;
      if (t_binary->binary_operator == tacky::Binary_Operator::Divide)
      {
        mov_reg2 = std::make_unique<aast::Reg>(aast::RegType::AX, aast::Size::DWORD); // quotient goes in eax
      }
      else if (t_binary->binary_operator == tacky::Binary_Operator::Remainder)
      {
        mov_reg2 = std::make_unique<aast::Reg>(aast::RegType::DX, aast::Size::DWORD); // remainder goes int edx
      }

      aast::Operand *mov_dst = generate_operand(t_binary->dst, operands);
      std::unique_ptr<aast::Mov> mov2 = std::make_unique<aast::Mov>(mov_reg2.get(), mov_dst); // copy register value to address
      assembly_instructions.push_back(std::move(mov2));
      operands.push_back(std::move(mov_reg1));
      operands.push_back(std::move(mov_reg2));
    }
    else if (std::find(logical_operators.begin(), logical_operators.end(), t_binary->binary_operator) != logical_operators.end())
    {
      aast::Operand *src1 = generate_operand(t_binary->src1, operands);
      aast::Operand *src2 = generate_operand(t_binary->src2, operands);
      std::unique_ptr<aast::Cmp> cmp = std::make_unique<aast::Cmp>(src2, src1);
      assembly_instructions.push_back(std::move(cmp));

      std::unique_ptr<aast::Imm> imm = std::make_unique<aast::Imm>(0);
      aast::Operand *dst = generate_operand(t_binary->dst, operands);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(imm.get(), dst);
      assembly_instructions.push_back(std::move(mov));
      operands.push_back(std::move(imm));

      aast::Cond_Code *code = generate_conditional_codes(t_binary->binary_operator);
      std::unique_ptr<aast::SetCC> set_cc = std::make_unique<aast::SetCC>(code, dst);
      assembly_instructions.push_back(std::move(set_cc));
    }
  }

  // This function converts compound operations from TACKY to assembly instructions
  void generate_com(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Compound *t_compound = dynamic_cast<tacky::Compound *>(instruction);
    if (!t_compound)
      return;
    if (std::find(basic_operators.begin(), basic_operators.end(), t_compound->binary_operator) != basic_operators.end())
    {
      // move the our src into register
      aast::Operand *dst = generate_operand(t_compound->dst, operands);
      std::unique_ptr<aast::Reg> reg1 = std::make_unique<aast::Reg>(aast::RegType::R10, aast::Size::DWORD);
      aast::Reg *reg = reg1.get();
      operands.push_back(std::move(reg1));

      std::unique_ptr<aast::Mov> mov1 = std::make_unique<aast::Mov>(dst, reg);
      assembly_instructions.push_back(std::move(mov1));

      // perform operation on register
      aast::Operand *bin_src = generate_operand(t_compound->src, operands);
      aast::Binary_Operator *bin_op = generate_basic_binary_operators(t_compound->binary_operator);
      std::unique_ptr<aast::Binary> binary = std::make_unique<aast::Binary>(bin_op, bin_src, reg);
      assembly_instructions.push_back(std::move(binary));

      // move register back to src
      std::unique_ptr<aast::Mov> mov2 = std::make_unique<aast::Mov>(reg, dst);
      assembly_instructions.push_back(std::move(mov2));
    }
    else if (std::find(complex_operators.begin(), complex_operators.end(), t_compound->binary_operator) != complex_operators.end())
    {
      // move dst to register
      aast::Operand *dst = generate_operand(t_compound->dst, operands); // dividend
      std::unique_ptr<aast::Reg> reg1 = std::make_unique<aast::Reg>(aast::RegType::AX, aast::Size::DWORD);

      std::unique_ptr<aast::Mov> mov1 = std::make_unique<aast::Mov>(dst, reg1.get());
      assembly_instructions.push_back(std::move(mov1));
      operands.push_back(std::move(reg1));

      // perform sign extension
      std::unique_ptr<aast::Cdq> cdq = std::make_unique<aast::Cdq>(); // sign extension since idiv can only take in 6 bit values
      assembly_instructions.push_back(std::move(cdq));

      // divide
      aast::Operand *src = generate_operand(t_compound->src, operands); // divisor
      std::unique_ptr<aast::Idiv> idiv = std::make_unique<aast::Idiv>(src);
      assembly_instructions.push_back(std::move(idiv));

      std::unique_ptr<aast::Reg> reg2;
      if (t_compound->binary_operator == tacky::Binary_Operator::Divide)
      {
        reg2 = std::make_unique<aast::Reg>(aast::RegType::AX, aast::Size::DWORD); // quotient goes in eax
      }
      else if (t_compound->binary_operator == tacky::Binary_Operator::Remainder)
      {
        reg2 = std::make_unique<aast::Reg>(aast::RegType::DX, aast::Size::DWORD); // remainder goes int edx
      }

      std::unique_ptr<aast::Mov> mov2 = std::make_unique<aast::Mov>(reg2.get(), dst); // copy register value to address
      assembly_instructions.push_back(std::move(mov2));
      operands.push_back(std::move(reg2));
    }
  }

  // This function converts conditional jumps from TACKY to assembly instructions
  void generate_jmp_if(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::JumpIfZero *jmp_if_zero = dynamic_cast<tacky::JumpIfZero *>(instruction);
    tacky::JumpIfNotZero *jmp_if_not_zero = dynamic_cast<tacky::JumpIfNotZero *>(instruction);
    if (!jmp_if_zero && !jmp_if_not_zero)
      return;

    aast::Operand *val = nullptr;
    aast::Identifier *target = nullptr;
    aast::Cond_Code *cond_code = nullptr;
    if (jmp_if_zero)
    {
      val = generate_operand(jmp_if_zero->condition, operands);
      target = new aast::Identifier(jmp_if_zero->target->name);
      cond_code = new aast::E();
    }
    else if (jmp_if_not_zero)
    {
      val = generate_operand(jmp_if_not_zero->condition, operands);
      target = new aast::Identifier(jmp_if_not_zero->target->name);
      cond_code = new aast::NE();
    }

    std::unique_ptr<aast::Imm> imm = std::make_unique<aast::Imm>(0);
    std::unique_ptr<aast::Cmp> cmp = std::make_unique<aast::Cmp>(imm.get(), val);
    operands.push_back(std::move(imm));
    assembly_instructions.push_back(std::move(cmp));

    std::unique_ptr<aast::JmpCC> jmp_cc = std::make_unique<aast::JmpCC>(cond_code, target);
    assembly_instructions.push_back(std::move(jmp_cc));
  }

  // This function converts a regular TACKY jump to a Assembly jump
  void generate_jmp(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Jump *jmp = dynamic_cast<tacky::Jump *>(instruction);
    if (!jmp)
      return;

    aast::Identifier *identifier = new aast::Identifier(jmp->target->name);
    std::unique_ptr<aast::Jmp> simple_jmp = std::make_unique<aast::Jmp>(identifier);
    assembly_instructions.push_back(std::move(simple_jmp));
  }

  // This function converts a regular TACKY label to a Assembly label
  void generate_label(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Label *label = dynamic_cast<tacky::Label *>(instruction);
    if (!label)
      return;

    aast::Identifier *identifier = new aast::Identifier(label->identifier->name);
    std::unique_ptr<aast::Label> a_label = std::make_unique<aast::Label>(identifier);
    assembly_instructions.push_back(std::move(a_label));
  }

  // This function converts a TACKY copy to a Mov instruction
  void generate_copy(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Copy *copy = dynamic_cast<tacky::Copy *>(instruction);
    if (!copy)
      return;

    aast::Operand *src = generate_operand(copy->src, operands);
    aast::Operand *dst = generate_operand(copy->dst, operands);
    std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(src, dst);
    assembly_instructions.push_back(std::move(mov));
  }

  // This function converts a TACKY function call to a Call instruction
  void generate_call(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Fun_Call *fun_call = dynamic_cast<tacky::Fun_Call *>(instruction);
    if (!fun_call)
      return;

    std::size_t argument_count = fun_call->args.size();
    std::size_t register_arguments_count = std::min(MAX_PARAMS_IN_REGISTERS, argument_count);
    std::size_t stack_arguments_count = argument_count > MAX_PARAMS_IN_REGISTERS ? argument_count - MAX_PARAMS_IN_REGISTERS : 0;
    int stack_padding = 0;
    if(stack_arguments_count % 2)
    {
      stack_padding = 8;
    }

    if(stack_padding != 0) {
      assembly_instructions.push_back(std::make_unique<aast::AllocateStack>(stack_padding));
    }

    for(std::size_t i{};i < register_arguments_count;++i) {
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(argument_registers[i], aast::Size::DWORD);
      aast::Operand *assembly_arg = generate_operand(fun_call->args[i], operands);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(assembly_arg, reg.get());
      assembly_instructions.push_back(std::move(mov));
      operands.push_back(std::move(reg));
    }

    std::unique_ptr<aast::Reg> qword_ax_reg = std::make_unique<aast::Reg>(aast::RegType::AX, aast::Size::QWORD);
    std::unique_ptr<aast::Reg> dword_ax_reg = std::make_unique<aast::Reg>(aast::RegType::AX, aast::Size::DWORD);
    for(std::size_t i = 1;i <= stack_arguments_count;++i) {
      aast::Operand *assembly_arg = generate_operand(fun_call->args[argument_count - i], operands);
      aast::Imm *imm = dynamic_cast<aast::Imm *>(assembly_arg);
      if(imm) {
        std::unique_ptr<aast::Push> push = std::make_unique<aast::Push>(assembly_arg);
        assembly_instructions.push_back(std::move(push));
      }
      else {
        std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(assembly_arg, dword_ax_reg.get());
        assembly_instructions.push_back(std::move(mov));
        std::unique_ptr<aast::Push> push = std::make_unique<aast::Push>(qword_ax_reg.get());
        assembly_instructions.push_back(std::move(push));
      }
    }

    aast::Identifier *identifier = new aast::Identifier(fun_call->fun_name->name);
    std::unique_ptr<aast::Call> call = std::make_unique<aast::Call>(identifier);
    assembly_instructions.push_back(std::move(call));

    int bytes_to_remove = 8 * stack_arguments_count + stack_padding;
    if(bytes_to_remove != 0) {
      assembly_instructions.push_back(std::make_unique<aast::DeallocateStack>(bytes_to_remove));
    }

    aast::Operand *assembly_dst = generate_operand(fun_call->dst, operands);
    std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(dword_ax_reg.get(), assembly_dst);
    assembly_instructions.push_back(std::move(mov));
    operands.push_back(std::move(qword_ax_reg));
    operands.push_back(std::move(dword_ax_reg));
  }

  // This function converts every tacky Instruction to a set of assembly instructions
  void generate_instructions(std::vector<std::unique_ptr<tacky::Instruction>> &body, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    for (std::unique_ptr<tacky::Instruction> &instruction : body)
    {
      generate_jmp_if(instruction.get(), instructions, operands);
      generate_return(instruction.get(), instructions, operands);
      generate_unary(instruction.get(), instructions, operands);
      generate_com(instruction.get(), instructions, operands);
      generate_bin(instruction.get(), instructions, operands);
      generate_jmp(instruction.get(), instructions);
      generate_label(instruction.get(), instructions);
      generate_copy(instruction.get(), instructions, operands);
      generate_call(instruction.get(), instructions, operands);
    }
  }

  // This function converts the temporary variable to a offset from the base caller address within the stack frame
  aast::Stack *replace_pseudo(
    aast::Pseudo *pseudo, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    std::string var(pseudo->identifier->text);
    if (!stack_manager.stack_offset.count(var))
    { // Only add New Locations Crreated
      stack_manager.total_bytes_to_reserve += 4;
      stack_manager.stack_offset[var] = -stack_manager.total_bytes_to_reserve;
      std::unique_ptr<aast::Stack> stack = std::make_unique<aast::Stack>(stack_manager.stack_offset[var]);
      stack_manager.stack_objs.insert({stack_manager.stack_offset[var], stack.get()});
      operands.push_back(std::move(stack));
    }
    return stack_manager.stack_objs[stack_manager.stack_offset[var]];
  }

  // This function assists in replacing pseudo variables for mov
  void fix_mov(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::Mov *mov = dynamic_cast<aast::Mov *>(it->get());
    if (!mov)
      return;
    aast::Pseudo *src_pseudo = dynamic_cast<aast::Pseudo *>(mov->src);
    aast::Pseudo *dst_pseudo = dynamic_cast<aast::Pseudo *>(mov->dst);
    aast::Data *src_data = dynamic_cast<aast::Data *>(mov->src);
    aast::Data *dst_data = dynamic_cast<aast::Data *>(mov->dst);
    aast::Stack *src_stack = dynamic_cast<aast::Stack *>(mov->src);
    aast::Stack *dst_stack = dynamic_cast<aast::Stack *>(mov->dst);
    if ((src_pseudo || src_stack || src_data) && (dst_pseudo || dst_stack || dst_data))
    { 
      // parameters will have a stack address as operand so we can't set it to nullptr if it isn't a pseudo
      if(src_pseudo) src_stack = replace_pseudo(src_pseudo, operands, stack_manager);
      if(dst_pseudo) dst_stack = replace_pseudo(dst_pseudo, operands, stack_manager);

      // separate into 2 instructions using r10d register
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::R10, aast::Size::DWORD);
      std::unique_ptr<aast::Mov> new_mov = std::make_unique<aast::Mov>(
        (src_stack ? static_cast<aast::Operand *>(src_stack) : static_cast<aast::Operand *>(src_data)), reg.get()
      ); // copies src to register

      mov->src = reg.get(); // replaces current temporary variable with register
      mov->dst = dst_stack ? static_cast<aast::Operand *>(dst_stack) : static_cast<aast::Operand *>(dst_data); 
      it = instructions.insert(it, std::move(new_mov)); // Inserts before the current Mov Instruction
      operands.push_back(std::move(reg));
    }
    else if (src_pseudo)
    {
      src_stack = replace_pseudo(src_pseudo, operands, stack_manager);
      mov->src = src_stack;
    }
    else if (dst_pseudo)
    {
      dst_stack = replace_pseudo(dst_pseudo, operands, stack_manager);
      mov->dst = dst_stack;
    }
  }

  // This function assists in replacing pseudo variables for unary operators
  void fix_unary(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::Unary *unary = dynamic_cast<aast::Unary *>(it->get());
    if (!unary)
      return;
    aast::Pseudo *operand = dynamic_cast<aast::Pseudo *>(unary->operand);
    if (operand)
    {
      aast::Stack *stack = replace_pseudo(operand, operands, stack_manager);
      unary->operand = stack;
    }
  }

  void fix_shifting(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(it->get());
    if (!binary)
      return;
    aast::Shl *shl = dynamic_cast<aast::Shl *>(binary->binary_operator);
    aast::Sar *sar = dynamic_cast<aast::Sar *>(binary->binary_operator);
    if (!shl && !sar)
      return;

    aast::Pseudo *operand1_pseudo = dynamic_cast<aast::Pseudo *>(binary->operand1);
    aast::Data *operand1_data = dynamic_cast<aast::Data *>(binary->operand1);
    if (operand1_pseudo || operand1_data)
    {
      aast::Stack *src_stack = operand1_pseudo ? replace_pseudo(operand1_pseudo, operands, stack_manager) : nullptr;
      std::unique_ptr<aast::Reg> reg1 = std::make_unique<aast::Reg>(aast::RegType::CX, aast::Size::DWORD);
      std::unique_ptr<aast::Reg> reg2 = std::make_unique<aast::Reg>(aast::RegType::CX, aast::Size::BYTE);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(
        src_stack ? static_cast<aast::Operand *>(src_stack) : static_cast<aast::Operand *>(operand1_data), reg1.get()
      );
      binary->operand1 = reg2.get();
      operands.push_back(std::move(reg1));
      operands.push_back(std::move(reg2));

      it = instructions.insert(it, std::move(mov));
    }
    aast::Pseudo *operand2_pseudo = dynamic_cast<aast::Pseudo *>(binary->operand2);
    aast::Data *operand2_data = dynamic_cast<aast::Data *>(binary->operand2);
    if (operand2_pseudo || operand2_data)
    {
      aast::Stack *dst_stack = operand2_pseudo ? replace_pseudo(operand2_pseudo, operands, stack_manager) : nullptr;
      binary->operand2 = dst_stack ? static_cast<aast::Operand *>(dst_stack) : static_cast<aast::Operand *>(operand2_data);
    }

    aast::Reg *reg = dynamic_cast<aast::Reg *>(binary->operand1);
    if (reg && reg->reg_size != aast::Size::BYTE && reg->reg_type != aast::RegType::CX)
    {
      std::unique_ptr<aast::Reg> reg1 = std::make_unique<aast::Reg>(aast::RegType::CX, aast::Size::DWORD);
      std::unique_ptr<aast::Reg> reg2 = std::make_unique<aast::Reg>(aast::RegType::CX, aast::Size::BYTE);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(reg, reg1.get());
      binary->operand1 = reg2.get();
      operands.push_back(std::move(reg1));
      operands.push_back(std::move(reg2));

      it = instructions.insert(it, std::move(mov));
    }
  }

  // This function assists in replacing pseudo variables for basic operators
  void fix_basic(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(it->get());
    if (!binary)
      return;
    aast::Mult *mult = dynamic_cast<aast::Mult *>(binary->binary_operator);
    if (mult)
      return;
    aast::Pseudo *operand1_pseudo = dynamic_cast<aast::Pseudo *>(binary->operand1);
    aast::Data *operand1_data = dynamic_cast<aast::Data *>(binary->operand1);
    aast::Pseudo *operand2_pseudo = dynamic_cast<aast::Pseudo *>(binary->operand2);
    aast::Data *operand2_data = dynamic_cast<aast::Data *>(binary->operand2);
    aast::Stack *src_stack = nullptr, *dst_stack = nullptr;
    if ((operand1_pseudo || operand1_data) && (operand2_pseudo || operand2_data))
    { // same as mov with 2 addresses
      src_stack = operand1_pseudo ? replace_pseudo(operand1_pseudo, operands, stack_manager) : nullptr;
      dst_stack = operand2_pseudo ? replace_pseudo(operand2_pseudo, operands, stack_manager) : nullptr;

      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::R10, aast::Size::DWORD);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(
        src_stack ? static_cast<aast::Operand *>(src_stack) : static_cast<aast::Operand *>(operand1_data), reg.get()
      );

      binary->operand1 = reg.get();
      binary->operand2 = dst_stack ? static_cast<aast::Operand *>(dst_stack) : static_cast<aast::Operand *>(operand2_data);
      it = instructions.insert(it, std::move(mov));
      operands.push_back(std::move(reg));
    }
    else if (operand1_pseudo)
    {
      src_stack = replace_pseudo(operand1_pseudo, operands, stack_manager);
      binary->operand1 = src_stack;
    }
    else if (operand2_pseudo)
    {
      dst_stack = replace_pseudo(operand2_pseudo, operands, stack_manager);
      binary->operand2 = dst_stack;
    }

    aast::Imm *dst = dynamic_cast<aast::Imm *>(binary->operand2);
    if (dst)
    {
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::R11, aast::Size::DWORD);
      binary->operand2 = reg.get();

      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(dst, reg.get());
      it = instructions.insert(it, std::move(mov));

      operands.push_back(std::move(reg));
    }
  }

  // This function assists in replacing pseudo variables for multiplication operator
  void fix_mult(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(it->get());
    if (!binary)
      return;
    aast::Mult *mult = dynamic_cast<aast::Mult *>(binary->binary_operator);
    if (!mult)
      return;
    aast::Pseudo *src_pseudo = dynamic_cast<aast::Pseudo *>(binary->operand1);
    aast::Data *src_data = dynamic_cast<aast::Data *>(binary->operand1);
    aast::Pseudo *dst_pseudo = dynamic_cast<aast::Pseudo *>(binary->operand2);
    aast::Data *dst_data = dynamic_cast<aast::Data *>(binary->operand2);
    if (src_pseudo || src_data)
    {
      aast::Stack *src_stack = src_pseudo ? replace_pseudo(src_pseudo, operands, stack_manager) : nullptr;
      binary->operand1 = src_stack ? static_cast<aast::Operand *>(src_stack) : static_cast<aast::Operand *>(src_data);
    }
    if (dst_pseudo || dst_data)
    {
      aast::Stack *dst_stack = dst_pseudo ? replace_pseudo(dst_pseudo, operands, stack_manager) : nullptr;

      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::R11, aast::Size::DWORD);
      std::unique_ptr<aast::Mov> mov1 = std::make_unique<aast::Mov>(
        dst_stack ? static_cast<aast::Operand *>(dst_stack) : static_cast<aast::Operand *>(dst_data), reg.get()
      ); // copies address content into register
      binary->operand2 = reg.get(); // multiplies constant by content in register
      instructions.insert(it, std::move(mov1));
      ++it;

      std::unique_ptr<aast::Mov> mov2 = std::make_unique<aast::Mov>(
        reg.get(), dst_stack ? static_cast<aast::Operand *>(dst_stack) : static_cast<aast::Operand *>(dst_data)
      ); // copies register content to the original address;
      it = instructions.insert(it, std::move(mov2));

      operands.push_back(std::move(reg));
    }
  }

  // This function assists in replacing pseudo variables for division and modulo operator
  void fix_div(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::Idiv *idiv = dynamic_cast<aast::Idiv *>(it->get());
    if (!idiv)
      return;
    aast::Pseudo *operand_pseudo = dynamic_cast<aast::Pseudo *>(idiv->operand);
    aast::Data *operand_data = dynamic_cast<aast::Data *>(idiv->operand);
    aast::Imm *imm_val = dynamic_cast<aast::Imm *>(idiv->operand);
    if (operand_pseudo || operand_data)
    {
      aast::Stack *stack = operand_pseudo ? replace_pseudo(operand_pseudo, operands, stack_manager) : nullptr;
      idiv->operand = stack ? static_cast<aast::Operand *>(stack) : static_cast<aast::Operand *>(operand_data);
    }
    else if (imm_val)
    {
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::R10, aast::Size::DWORD);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(imm_val, reg.get()); // copies divisor into a register and then apply the division onto that register directly
      // aast::Mov Instruction uses its immediate value so we can't delete
      idiv->operand = reg.get();
      it = instructions.insert(it, std::move(mov));
      operands.push_back(std::move(reg));
    }
  }

  // This function assists in replacing pseudos for cmp as well as rearranging its operands
  void fix_cmp(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::Cmp *cmp = dynamic_cast<aast::Cmp *>(it->get());
    if (!cmp)
      return;
    aast::Pseudo *operand1_pseudo = dynamic_cast<aast::Pseudo *>(cmp->operand1);
    aast::Data *operand1_data = dynamic_cast<aast::Data *>(cmp->operand1);
    aast::Pseudo *operand2_pseudo = dynamic_cast<aast::Pseudo *>(cmp->operand2);
    aast::Data *operand2_data = dynamic_cast<aast::Data *>(cmp->operand2);
    aast::Stack *src_stack = nullptr,
                *dst_stack = nullptr;
    if ((operand1_pseudo || operand1_data) && (operand2_pseudo || operand2_data))
    {
      src_stack = operand1_pseudo ? replace_pseudo(operand1_pseudo, operands, stack_manager) : nullptr;
      dst_stack = operand2_pseudo ? replace_pseudo(operand2_pseudo, operands, stack_manager) : nullptr;

      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::R10, aast::Size::DWORD);
      cmp->operand1 = reg.get();
      cmp->operand2 = dst_stack ? static_cast<aast::Operand *>(dst_stack) : static_cast<aast::Operand *>(operand2_data);

      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(
        src_stack ? static_cast<aast::Operand *>(src_stack) : static_cast<aast::Operand *>(operand1_data), reg.get()
      );
      it = instructions.insert(it, std::move(mov));

      operands.push_back(std::move(reg));
    }
    else if (operand1_pseudo)
    {
      src_stack = replace_pseudo(operand1_pseudo, operands, stack_manager);
      cmp->operand1 = src_stack;
    }
    else if (operand2_pseudo)
    {
      dst_stack = replace_pseudo(operand2_pseudo, operands, stack_manager);
      cmp->operand2 = dst_stack;
    }

    aast::Imm *dst = dynamic_cast<aast::Imm *>(cmp->operand2);
    if (dst)
    {
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(aast::RegType::R11, aast::Size::DWORD);

      std::unique_ptr<aast::Imm> mov_copy = std::make_unique<aast::Imm>(dst->val);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(mov_copy.get(), reg.get());
      it = instructions.insert(it, std::move(mov));

      cmp->operand2 = reg.get();

      operands.push_back(std::move(reg));
      operands.push_back(std::move(mov_copy));
    }
  }

  // This function handles setcc instructions with registers
  void fix_set(
    typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, 
    std::vector<std::unique_ptr<aast::Operand>> &operands,
    StackManager &stack_manager
  )
  {
    aast::SetCC *setcc = dynamic_cast<aast::SetCC *>(it->get());
    if (!setcc)
      return;
    aast::Pseudo *operand = dynamic_cast<aast::Pseudo *>(setcc->operand);
    if (operand)
    {
      aast::Stack *stack = replace_pseudo(operand, operands, stack_manager);
      setcc->operand = stack;
    }
  }

  // This function converts every temporary Variable (Pseudo) to a stack offset
  void compiler_pass(
    StackManager &stack_manager, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands
  )
  {
    // all of the replacements replace aast::Pseudo with aast::Stack or aast::Data
    for (typename std::list<std::unique_ptr<aast::Instruction>>::iterator it = instructions.begin(); it != instructions.end(); ++it)
    {
      fix_mov(it, instructions, operands, stack_manager);
      fix_unary(it, operands, stack_manager);
      fix_shifting(it, instructions, operands, stack_manager);
      fix_cmp(it, instructions, operands, stack_manager);
      fix_set(it, operands, stack_manager);
      fix_mult(it, instructions, operands, stack_manager);
      fix_basic(it, instructions, operands, stack_manager);
      fix_div(it, instructions, operands, stack_manager);
    }

    int stack_size = static_cast<int>((stack_manager.total_bytes_to_reserve + 15) & ~std::size_t(15));// rounds to the nearest multiple of 16
    std::unique_ptr<aast::AllocateStack> allocateStack = std::make_unique<aast::AllocateStack>(stack_size); // Reserves memory in stack frame for local variables
    instructions.push_front(std::move(allocateStack));
  }

  void copy_parameters(
    std::vector<std::unique_ptr<tacky::Identifier>> &params, 
    std::list<std::unique_ptr<aast::Instruction>> &instructions, 
    std::vector<std::unique_ptr<aast::Operand>> &operands
  )
  {
    std::size_t n = params.size();
    for(std::size_t i = 0;i < std::min(MAX_PARAMS_IN_REGISTERS, n);++i) {
      aast::Identifier *param_name = new aast::Identifier(params[i]->name);
      std::unique_ptr<aast::Pseudo> dst = std::make_unique<aast::Pseudo>(param_name);
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(argument_registers[i], aast::Size::DWORD);;
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(reg.get(), dst.get());
      instructions.push_back(std::move(mov));
      operands.push_back(std::move(reg));
      operands.push_back(std::move(dst));
    }

    for(std::size_t i = MAX_PARAMS_IN_REGISTERS; i < n; ++i) {
      int stack_arguement_offset = 16 + 8 * static_cast<int>(i - MAX_PARAMS_IN_REGISTERS);
      aast::Identifier *param_name = new aast::Identifier(params[i]->name);
      std::unique_ptr<aast::Pseudo> pseudo = std::make_unique<aast::Pseudo>(param_name);
      std::unique_ptr<aast::Stack> stack = std::make_unique<aast::Stack>(stack_arguement_offset);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(stack.get(), pseudo.get());
      instructions.push_back(std::move(mov));
      operands.push_back(std::move(stack));
      operands.push_back(std::move(pseudo));
    }
  }

  aast::Function generate_function(tacky::Function *func)
  {
    std::vector<std::unique_ptr<aast::Operand>> operands;
    std::list<std::unique_ptr<aast::Instruction>> instructions;

    // parameters
    copy_parameters(func->params, instructions, operands);

    // body
    generate_instructions(func->body, instructions, operands);

    // replace pseudoregisters
    StackManager stack_manager;
    compiler_pass(stack_manager, instructions, operands); // Convert after We allocate all needed temporary Variables

    return aast::Function(std::make_unique<aast::Identifier>(func->identifier->name), std::move(instructions), std::move(operands), func->global);
  }

  aast::Static_Variable generate_static_variable(tacky::Static_Variable *static_variable)
  {
    return aast::Static_Variable(std::make_unique<aast::Identifier>(static_variable->identifier->name), static_variable->init, static_variable->global);
  }

  // This function converts Tacky nodes to assembly instructions
  aast::Program *generate_top_level(tacky::Program *tacky_program)
  {
    std::vector<aast::Top_Level> top_levels;
    for(auto &top_level : tacky_program->top_levels)
    {
      if(std::holds_alternative<tacky::Function>(top_level)) {
        top_levels.emplace_back(generate_function(std::get_if<tacky::Function>(&top_level)));
      } else if(std::holds_alternative<tacky::Static_Variable>(top_level)) {
        top_levels.emplace_back(generate_static_variable(std::get_if<tacky::Static_Variable>(&top_level)));
      }
    }
    aast::Program *program = new aast::Program(std::move(top_levels));
    return program;
  }
}
