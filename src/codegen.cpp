#include "aast/aast.hpp"
#include "tacky/tacky.hpp"
#include "compiler/codegen.hpp"
#include <ostream>
#include <list>
#include <unordered_map>

namespace codegen
{
  std::unordered_map<std::string, int> stack_offset; // unique rbp jumps
  uint32_t total_bytes_to_reserve = 0;               // total bytes to allocate to aast::Stack Frame

  // This function converts Tacky Values to Immediate Values and Temporary Variables
  aast::Operand *generate_operand(tacky::Val *&t_val)
  {
    tacky::Constant *t_constant = dynamic_cast<tacky::Constant *>(t_val);
    tacky::Var *t_var = dynamic_cast<tacky::Var *>(t_val);
    if (t_constant)
    {
      aast::Imm *immediate_value = new aast::Imm(t_constant->val);
      return immediate_value;
    }
    else if (t_var)
    {
      aast::Identifier *assembly_identifier = new aast::Identifier(t_var->identifier->name);
      aast::Pseudo *pseudo_identifier = new aast::Pseudo(assembly_identifier);
      return pseudo_identifier;
    }
    return nullptr;
  }

  // This function converts Tacky unary operators to assembly instructions for unary operators
  aast::Unary_Operator *generate_unary_operators(tacky::Unary_Operator *unary_operator)
  {
    tacky::Complement *t_complement = dynamic_cast<tacky::Complement *>(unary_operator);
    tacky::Negate *t_negate = dynamic_cast<tacky::Negate *>(unary_operator);
    aast::Unary_Operator *res = nullptr;
    if (t_complement)
    {
      res = new aast::Not();
    }
    else if (t_negate)
    {
      res = new aast::Neg();
    }
    return res;
  }

  // This function converts from TACKY to assembly Instructions for Binary Operators +, -, *
  aast::Binary_Operator *generate_basic_binary_operators(tacky::Binary_Operator *binary_operator)
  {
    tacky::Add *t_add = dynamic_cast<tacky::Add *>(binary_operator);
    tacky::Subtract *t_sub = dynamic_cast<tacky::Subtract *>(binary_operator);
    tacky::Multiply *t_mult = dynamic_cast<tacky::Multiply *>(binary_operator);
    aast::Binary_Operator *res = nullptr;
    if (t_add)
    {
      res = new aast::Add();
    }
    else if (t_sub)
    {
      res = new aast::Sub();
    }
    else if (t_mult)
    {
      res = new aast::Mult();
    }

    return res;
  }

  // This function creates the assembly Return Instruction
  void generate_return(tacky::Return *t_return, std::list<aast::Instruction *> &assembly_instructions)
  {
    aast::Operand *val = generate_operand(t_return->val);
    aast::AX *ax_register = new aast::AX();
    aast::Reg *reg = new aast::Reg(ax_register);
    aast::Mov *move = new aast::Mov(val, reg);
    assembly_instructions.push_back(move);

    aast::Ret *ret = new aast::Ret();
    assembly_instructions.push_back(ret);
  }

  // This function creates the assembly Instruction for tacky Unary Operators
  void generate_unary(tacky::Unary *t_unary, std::list<aast::Instruction *> &assembly_instructions)
  {
    aast::Operand *src = generate_operand(t_unary->src);
    aast::Operand *dst = generate_operand(t_unary->dst);
    aast::Mov *mov = new aast::Mov(src, dst);
    assembly_instructions.push_back(mov);

    aast::Operand *operand = generate_operand(t_unary->dst); // tacky::his is required to avoid dangling pointer later
    aast::Unary_Operator *unary_operator = generate_unary_operators(t_unary->unary_operator);
    aast::Unary *unary = new aast::Unary(unary_operator, operand);
    assembly_instructions.push_back(unary);
  }

  // This function creates the assembly Instruction for tacky Binary Operators +, -, *
  void generate_bin_basic(tacky::Binary *t_binary, std::list<aast::Instruction *> &assembly_instructions)
  {
    aast::Operand *mov_src = generate_operand(t_binary->src1);
    aast::Operand *mov_dst = generate_operand(t_binary->dst);
    aast::Mov *mov = new aast::Mov(mov_src, mov_dst);
    assembly_instructions.push_back(mov);

    aast::Operand *bin_src = generate_operand(t_binary->src2);
    aast::Operand *bin_dst = generate_operand(t_binary->dst);
    aast::Binary_Operator *bin_op = generate_basic_binary_operators(t_binary->binary_operator);
    aast::Binary *binary = new aast::Binary(bin_op, bin_src, bin_dst);
    assembly_instructions.push_back(binary);
  }

  // This function creates the assembly Instruction for Tacky Binary Operators /, %
  void generate_bin_div(tacky::Binary *t_binary, std::list<aast::Instruction *> &assembly_instructions)
  {
    tacky::Divide *div = dynamic_cast<tacky::Divide *>(t_binary->binary_operator);
    tacky::Remainder *remainder = dynamic_cast<tacky::Remainder *>(t_binary->binary_operator);

    aast::RegType *reg_type1 = new aast::AX(), *reg_type2 = nullptr;
    if (div)
    {
      reg_type2 = new aast::AX(); // quotient goes in eax
    }
    else if (remainder)
    {
      reg_type2 = new aast::DX(); // remainder goes int edx
    }
    aast::Reg *mov1_reg = new aast::Reg(reg_type1);

    aast::Operand *mov_src = generate_operand(t_binary->src1); // dividend
    aast::Mov *mov1 = new aast::Mov(mov_src, mov1_reg);
    assembly_instructions.push_back(mov1);

    aast::Cdq *cdq = new aast::Cdq(); // sign extension since idiv can only take in 64 bit values
    assembly_instructions.push_back(cdq);

    aast::Operand *div_src = generate_operand(t_binary->src2); // divisor
    aast::Idiv *idiv = new aast::Idiv(div_src);
    assembly_instructions.push_back(idiv);

    aast::Reg *mov2_reg = new aast::Reg(reg_type2);
    aast::Operand *mov_dst = generate_operand(t_binary->dst);
    aast::Mov *mov2 = new aast::Mov(mov2_reg, mov_dst); // copy register value to address
    assembly_instructions.push_back(mov2);
  }

  // This function converts every tacky Instruction to a set of assembly instructions
  std::list<aast::Instruction *> generate_instructions(tacky::Function *func)
  {
    std::list<aast::Instruction *> assembly_instructions;
    for (tacky::Instruction *&instruction : func->body)
    {
      tacky::Return *t_return = dynamic_cast<tacky::Return *>(instruction);
      tacky::Unary *t_unary = dynamic_cast<tacky::Unary *>(instruction);
      tacky::Binary *t_binary = dynamic_cast<tacky::Binary *>(instruction);
      if (t_return)
      {
        generate_return(t_return, assembly_instructions);
      }
      else if (t_unary)
      {
        generate_unary(t_unary, assembly_instructions);
      }
      else if (t_binary)
      {
        tacky::Divide *div = dynamic_cast<tacky::Divide *>(t_binary->binary_operator);
        tacky::Remainder *remainder = dynamic_cast<tacky::Remainder *>(t_binary->binary_operator);
        if (div || remainder)
        {
          generate_bin_div(t_binary, assembly_instructions);
        }
        else
        {
          generate_bin_basic(t_binary, assembly_instructions);
        }
      }
    }

    return assembly_instructions;
  }

  // This function converts the temporary variable to a offset from the base caller address within the stack frame
  aast::Stack *replace_pseudo(aast::Pseudo *pseudo)
  {
    std::string var(pseudo->identifier->name);
    if (!stack_offset.count(var))
    { // Only aast::dd New Locations Crreated
      total_bytes_to_reserve += 4;
      stack_offset[var] = -total_bytes_to_reserve;
    }
    aast::Stack *stack = new aast::Stack(stack_offset[var]);
    return stack;
  }

  // This function assists in replacing pseudo variables for mov
  void fix_mov(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions)
  {
    aast::Mov *mov = dynamic_cast<aast::Mov *>(*it);
    if (!mov)
      return;
    aast::Pseudo *src = dynamic_cast<aast::Pseudo *>(mov->src);
    aast::Pseudo *dst = dynamic_cast<aast::Pseudo *>(mov->dst);
    aast::Stack *src_stack = nullptr, *dst_stack = nullptr;
    if (src && dst)
    { // separate into 2 instructions using r10d register
      src_stack = replace_pseudo(src);
      dst_stack = replace_pseudo(dst);

      aast::R10 *r10_1 = new aast::R10(), *r10_2 = new aast::R10();
      aast::Reg *register_1 = new aast::Reg(r10_1), *register_2 = new aast::Reg(r10_2);
      aast::Mov *new_mov = new aast::Mov(src_stack, register_1); // copies src to register

      delete mov->src;
      mov->src = register_2; // replaces current temporary variable with register
      delete mov->dst;
      mov->dst = dst_stack;                  // new address
      it = instructions.insert(it, new_mov); // Inserts before the current Mov Instruction
    }
    else if (src)
    {
      src_stack = replace_pseudo(src);
      delete mov->src;
      mov->src = src_stack;
    }
    else if (dst)
    {
      dst_stack = replace_pseudo(dst);
      delete mov->dst;
      mov->dst = dst_stack;
    }
  }

  // This function assists in replacing pseudo variables for unary operators
  void fix_unary(typename std::list<aast::Instruction *>::iterator &it)
  {
    aast::Unary *unary = dynamic_cast<aast::Unary *>(*it);
    if (!unary)
      return;
    aast::Pseudo *operand = dynamic_cast<aast::Pseudo *>(unary->operand);
    if (operand)
    {
      aast::Stack *stack = replace_pseudo(operand);
      delete unary->operand;
      unary->operand = stack;
    }
  }

  // This function assists in replacing pseudo variables for addiiton and subtraction operators
  void fix_add_sub(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions)
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(*it);
    if (!binary)
      return;
    aast::Mult *mult = dynamic_cast<aast::Mult *>(binary->binary_operator);
    if (mult)
      return;
    aast::Pseudo *operand1 = dynamic_cast<aast::Pseudo *>(binary->operand1);
    aast::Pseudo *operand2 = dynamic_cast<aast::Pseudo *>(binary->operand2);
    aast::Stack *src_stack = nullptr, *dst_stack = nullptr;
    if (operand1 && operand2)
    { // same as mov with 2 addresses
      src_stack = replace_pseudo(operand1);
      dst_stack = replace_pseudo(operand2);

      aast::R10 *r10_1 = new aast::R10(), *r10_2 = new aast::R10();
      aast::Reg *reg1 = new aast::Reg(r10_1), *reg2 = new aast::Reg(r10_2);
      aast::Mov *mov = new aast::Mov(src_stack, reg1);

      delete binary->operand1;
      binary->operand1 = reg2;
      delete binary->operand2;
      binary->operand2 = dst_stack;
      it = instructions.insert(it, mov);
    }
    else if (operand1)
    {
      src_stack = replace_pseudo(operand1);
      delete binary->operand1;
      binary->operand1 = src_stack;
    }
    else if (operand2)
    {
      dst_stack = replace_pseudo(operand2);
      delete binary->operand2;
      binary->operand2 = dst_stack;
    }
  }

  // This function assists in replacing pseudo variables for multiplication operator
  void fix_mult(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions)
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(*it);
    if (!binary)
      return;
    aast::Mult *mult = dynamic_cast<aast::Mult *>(binary->binary_operator);
    if (!mult)
      return;
    aast::Pseudo *src = dynamic_cast<aast::Pseudo *>(binary->operand1);
    aast::Pseudo *dst = dynamic_cast<aast::Pseudo *>(binary->operand2);
    aast::Stack *mov1_stack = nullptr, *mov2_stack = nullptr;
    if (src)
    {
      aast::Stack *src_stack = replace_pseudo(src);
      delete binary->operand1;
      binary->operand1 = src_stack;
    }
    if (dst)
    {
      mov1_stack = replace_pseudo(dst);
      mov2_stack = replace_pseudo(dst);

      aast::R11 *r11_1 = new aast::R11(), *r11_2 = new aast::R11(), *r11_3 = new aast::R11();
      aast::Reg *reg1 = new aast::Reg(r11_1), *reg2 = new aast::Reg(r11_2), *reg3 = new aast::Reg(r11_3);
      aast::Mov *mov1 = new aast::Mov(mov1_stack, reg1); // copies address content into register
      delete binary->operand2;
      binary->operand2 = reg2; // multiplies constant by content in register
      instructions.insert(it, mov1);
      ++it;

      aast::Mov *mov2 = new aast::Mov(reg3, mov2_stack); // copies register content to the original address;
      it = instructions.insert(it, mov2);
    }
  }

  // This function assists in replacing pseudo variables for division and modulo operator
  void fix_div(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions)
  {
    aast::Idiv *idiv = dynamic_cast<aast::Idiv *>(*it);
    if (!idiv)
      return;
    aast::Pseudo *pseudo = dynamic_cast<aast::Pseudo *>(idiv->operand);
    aast::Imm *imm_val = dynamic_cast<aast::Imm *>(idiv->operand);
    if (pseudo)
    {
      aast::Stack *stack = replace_pseudo(pseudo);
      delete idiv->operand;
      idiv->operand = stack;
    }
    else if (imm_val)
    {
      aast::R10 *r10_1 = new aast::R10(), *r10_2 = new aast::R10();
      aast::Reg *reg1 = new aast::Reg(r10_1), *reg2 = new aast::Reg(r10_2);
      aast::Mov *mov = new aast::Mov(imm_val, reg1); // copies divisor into a register and then apply the division onto that register directly
      // delete idiv->operand; aast::Mov Instruction uses its immediate value so we can't delete
      idiv->operand = reg2;
      it = instructions.insert(it, mov);
    }
  }

  // This function converts every temporary Variable (Pseudo) to a stack offset
  void compiler_pass(std::list<aast::Instruction *> &instructions)
  {
    // aast::ll of the replacements follow the same steps: create aast::Stack struct, delete aast::Pseudo, replace with aast::Stack struct
    for (typename std::list<aast::Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
    {
      fix_mov(it, instructions);
      fix_unary(it);
      fix_mult(it, instructions);
      fix_add_sub(it, instructions);
      fix_div(it, instructions);
    }

    aast::AllocateStack *allocateStack = new aast::AllocateStack(total_bytes_to_reserve); // Reserves memory in stack frame for local variables
    instructions.push_front(allocateStack);
  }

  // This function converts Tacky nodes to assembly instructions
  aast::Program *generate_top_level(tacky::Program *&tacky_program)
  {
    aast::Identifier *identifier = new aast::Identifier(tacky_program->func->identifier->name);
    std::list<aast::Instruction *> instructions = generate_instructions(tacky_program->func);
    compiler_pass(instructions); // Convert aast::fter We aast::llocate aast::ll Needed tacky::emporary Variables
    aast::Function *function = new aast::Function(identifier, instructions);
    aast::Program *program = new aast::Program(function);
    return program;
  }
}
