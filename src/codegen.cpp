#include "aast/aast.hpp"
#include "tacky/tacky.hpp"
#include "compiler/codegen.hpp"
#include <ostream>
#include <list>
#include <unordered_map>
#include <memory>
#include <utility>

namespace codegen
{
  std::unordered_map<std::string, int> stack_offset;               // unique rbp jumps
  std::unordered_map<int, std::shared_ptr<aast::Stack>> addresses; // the stack object
  uint32_t total_bytes_to_reserve = 0;                             // total bytes to allocate to aast::Stack Frame

  // This function converts Tacky Values to Immediate Values and Temporary Variables
  std::shared_ptr<aast::Operand> generate_operand(std::shared_ptr<tacky::Val> t_val)
  {
    tacky::Constant *t_constant = dynamic_cast<tacky::Constant *>(t_val.get());
    tacky::Var *t_var = dynamic_cast<tacky::Var *>(t_val.get());
    if (t_constant)
    {
      std::shared_ptr<aast::Imm> immediate_value = std::make_shared<aast::Imm>(t_constant->val);
      return immediate_value;
    }
    else if (t_var)
    {
      std::shared_ptr<aast::Identifier> assembly_identifier = std::make_shared<aast::Identifier>(t_var->identifier->name);
      std::shared_ptr<aast::Pseudo> pseudo_identifier = std::make_shared<aast::Pseudo>(std::move(assembly_identifier));
      return pseudo_identifier;
    }
    return nullptr;
  }

  // This function converts Tacky unary operators to assembly instructions for unary operators
  std::unique_ptr<aast::Unary_Operator> generate_unary_operators(std::unique_ptr<tacky::Unary_Operator> unary_operator)
  {
    tacky::Complement *t_complement = dynamic_cast<tacky::Complement *>(unary_operator.get());
    tacky::Negate *t_negate = dynamic_cast<tacky::Negate *>(unary_operator.get());
    tacky::Not *t_not = dynamic_cast<tacky::Not *>(unary_operator.get());
    std::unique_ptr<aast::Unary_Operator> res;
    if (t_complement)
    {
      res = std::make_unique<aast::Not>();
    }
    else if (t_negate)
    {
      res = std::make_unique<aast::Neg>();
    }
    else if (t_not)
    {
      res = std::make_unique<aast::Not>();
    }
    return res;
  }

  // This function converts from TACKY to assembly Instructions for Binary Operators +, -, *
  std::unique_ptr<aast::Binary_Operator> generate_basic_binary_operators(std::unique_ptr<tacky::Binary_Operator> binary_operator)
  {
    tacky::Add *t_add = dynamic_cast<tacky::Add *>(binary_operator.get());
    tacky::Subtract *t_sub = dynamic_cast<tacky::Subtract *>(binary_operator.get());
    tacky::Multiply *t_mult = dynamic_cast<tacky::Multiply *>(binary_operator.get());
    std::unique_ptr<aast::Binary_Operator> res;
    if (t_add)
    {
      res = std::make_unique<aast::Add>();
    }
    else if (t_sub)
    {
      res = std::make_unique<aast::Sub>();
    }
    else if (t_mult)
    {
      res = std::make_unique<aast::Mult>();
    }

    return res;
  }

  // This function converts the relational operators into the assembly code
  std::unique_ptr<aast::Cond_Code> generate_conditional_codes(std::unique_ptr<tacky::Binary_Operator> binary_operator)
  {
    tacky::Equal *eq = dynamic_cast<tacky::Equal *>(binary_operator.get());
    tacky::NotEqual *neq = dynamic_cast<tacky::NotEqual *>(binary_operator.get());
    tacky::LessThan *lt = dynamic_cast<tacky::LessThan *>(binary_operator.get());
    tacky::LessOrEqual *lte = dynamic_cast<tacky::LessOrEqual *>(binary_operator.get());
    tacky::GreaterThan *gt = dynamic_cast<tacky::GreaterThan *>(binary_operator.get());
    tacky::GreaterOrEqual *gte = dynamic_cast<tacky::GreaterOrEqual *>(binary_operator.get());
    std::unique_ptr<aast::Cond_Code> res;
    if (eq)
      res = std::make_unique<aast::E>();
    else if (neq)
      res = std::make_unique<aast::NE>();
    else if (lt)
      res = std::make_unique<aast::L>();
    else if (lte)
      res = std::make_unique<aast::LE>();
    else if (gt)
      res = std::make_unique<aast::G>();
    else if (gte)
      res = std::make_unique<aast::GE>();

    return res;
  }

  // This function creates the assembly Return Instruction
  void generate_return(std::unique_ptr<tacky::Instruction> &instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Return *t_return = dynamic_cast<tacky::Return *>(instruction.get());
    if (!t_return)
      return;
    std::shared_ptr<aast::Operand> val = generate_operand(std::move(t_return->val));
    std::shared_ptr<aast::AX> ax_register = std::make_shared<aast::AX>();
    std::shared_ptr<aast::Reg> reg = std::make_shared<aast::Reg>(std::move(ax_register));
    std::unique_ptr<aast::Mov> move = std::make_unique<aast::Mov>(std::move(val), std::move(reg));
    assembly_instructions.push_back(std::move(move));

    std::unique_ptr<aast::Ret> ret = std::make_unique<aast::Ret>();
    assembly_instructions.push_back(std::move(ret));
  }

  // This function creates the assembly Instruction for tacky Unary Operators
  void generate_unary(std::unique_ptr<tacky::Instruction> &instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Unary *t_unary = dynamic_cast<tacky::Unary *>(instruction.get());
    if (!t_unary)
      return;
    tacky::Not *t_not = dynamic_cast<tacky::Not *>(t_unary->unary_operator.get());
    std::shared_ptr<aast::Operand> src = generate_operand(t_unary->src);
    std::shared_ptr<aast::Operand> dst = generate_operand(t_unary->dst);
    if (t_not)
    {
      std::shared_ptr<aast::Imm> imm = std::make_shared<aast::Imm>(0);
      std::unique_ptr<aast::Cmp> cmp = std::make_unique<aast::Cmp>(imm, src);
      assembly_instructions.push_back(std::move(cmp));

      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(imm, dst);
      assembly_instructions.push_back(std::move(mov));

      std::unique_ptr<aast::E> eq = std::make_unique<aast::E>();
      std::unique_ptr<aast::SetCC> set_cc = std::make_unique<aast::SetCC>(std::move(eq), dst);
      assembly_instructions.push_back(std::move(set_cc));
    }
    else
    {
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(src, dst);
      assembly_instructions.push_back(std::move(mov));

      std::unique_ptr<aast::Unary_Operator> unary_operator = generate_unary_operators(std::move(t_unary->unary_operator));
      std::unique_ptr<aast::Unary> unary = std::make_unique<aast::Unary>(std::move(unary_operator), dst);
      assembly_instructions.push_back(std::move(unary));
    }
  }

  // This function creates the assembly Instruction for tacky Binary Operators
  void generate_bin(std::unique_ptr<tacky::Instruction> &instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Binary *t_binary = dynamic_cast<tacky::Binary *>(instruction.get());
    if (!t_binary)
      return;
    tacky::Add *add = dynamic_cast<tacky::Add *>(t_binary->binary_operator.get());
    tacky::Subtract *subt = dynamic_cast<tacky::Subtract *>(t_binary->binary_operator.get());
    tacky::Multiply *mult = dynamic_cast<tacky::Multiply *>(t_binary->binary_operator.get());
    tacky::Divide *div = dynamic_cast<tacky::Divide *>(t_binary->binary_operator.get());
    tacky::Remainder *remainder = dynamic_cast<tacky::Remainder *>(t_binary->binary_operator.get());
    tacky::Equal *eq = dynamic_cast<tacky::Equal *>(t_binary->binary_operator.get());
    tacky::NotEqual *neq = dynamic_cast<tacky::NotEqual *>(t_binary->binary_operator.get());
    tacky::LessThan *lt = dynamic_cast<tacky::LessThan *>(t_binary->binary_operator.get());
    tacky::LessOrEqual *lte = dynamic_cast<tacky::LessOrEqual *>(t_binary->binary_operator.get());
    tacky::GreaterThan *gt = dynamic_cast<tacky::GreaterThan *>(t_binary->binary_operator.get());
    tacky::GreaterOrEqual *gte = dynamic_cast<tacky::GreaterOrEqual *>(t_binary->binary_operator.get());
    if (add || subt || mult)
    {
      std::shared_ptr<aast::Operand> mov_src = generate_operand(std::move(t_binary->src1));
      std::shared_ptr<aast::Operand> dst = generate_operand(std::move(t_binary->dst));
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(std::move(mov_src), dst);
      assembly_instructions.push_back(std::move(mov));

      std::shared_ptr<aast::Operand> bin_src = generate_operand(std::move(t_binary->src2));
      std::unique_ptr<aast::Binary_Operator> bin_op = generate_basic_binary_operators(std::move(t_binary->binary_operator));
      std::unique_ptr<aast::Binary> binary = std::make_unique<aast::Binary>(std::move(bin_op), std::move(bin_src), dst);
      assembly_instructions.push_back(std::move(binary));
    }
    else if (div || remainder)
    {
      std::shared_ptr<aast::RegType> reg_type1 = std::make_shared<aast::AX>(), reg_type2;
      if (div)
      {
        reg_type2 = reg_type1; // quotient goes in eax
      }
      else if (remainder)
      {
        reg_type2 = std::make_shared<aast::DX>(); // remainder goes int edx
      }
      std::shared_ptr<aast::Reg> mov1_reg = std::make_shared<aast::Reg>(reg_type1);

      std::shared_ptr<aast::Operand> mov_src = generate_operand(std::move(t_binary->src1)); // dividend
      std::unique_ptr<aast::Mov> mov1 = std::make_unique<aast::Mov>(std::move(mov_src), std::move(mov1_reg));
      assembly_instructions.push_back(std::move(mov1));

      std::unique_ptr<aast::Cdq> cdq = std::make_unique<aast::Cdq>(); // sign extension since idiv can only take in 6 bit values
      assembly_instructions.push_back(std::move(cdq));

      std::shared_ptr<aast::Operand> div_src = generate_operand(std::move(t_binary->src2)); // divisor
      std::unique_ptr<aast::Idiv> idiv = std::make_unique<aast::Idiv>(std::move(div_src));
      assembly_instructions.push_back(std::move(idiv));

      std::shared_ptr<aast::Reg> mov2_reg = std::make_shared<aast::Reg>(std::move(reg_type2));
      std::shared_ptr<aast::Operand> mov_dst = generate_operand(std::move(t_binary->dst));
      std::unique_ptr<aast::Mov> mov2 = std::make_unique<aast::Mov>(std::move(mov2_reg), std::move(mov_dst)); // copy register value to address
      assembly_instructions.push_back(std::move(mov2));
    }
    else if (eq || neq || lt || lte || gt || gte)
    {
      std::shared_ptr<aast::Operand> src1 = generate_operand(std::move(t_binary->src1));
      std::shared_ptr<aast::Operand> src2 = generate_operand(std::move(t_binary->src2));
      std::unique_ptr<aast::Cmp> cmp = std::make_unique<aast::Cmp>(std::move(src2), std::move(src1));
      assembly_instructions.push_back(std::move(cmp));

      std::shared_ptr<aast::Imm> imm = std::make_shared<aast::Imm>(0);
      std::shared_ptr<aast::Operand> dst = generate_operand(std::move(t_binary->dst));
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(std::move(imm), dst);
      assembly_instructions.push_back(std::move(mov));

      std::unique_ptr<aast::Cond_Code> code = generate_conditional_codes(std::move(t_binary->binary_operator));
      std::unique_ptr<aast::SetCC> set_cc = std::make_unique<aast::SetCC>(std::move(code), dst);
      assembly_instructions.push_back(std::move(set_cc));
    }
  }

  // This function converts conditional jumps from TACKY to assembly instructions
  void generate_jmp_if(std::unique_ptr<tacky::Instruction> &instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::JumpIfZero *jmp_if_zero = dynamic_cast<tacky::JumpIfZero *>(instruction.get());
    tacky::JumpIfNotZero *jmp_if_not_zero = dynamic_cast<tacky::JumpIfNotZero *>(instruction.get());
    if (!jmp_if_zero && !jmp_if_not_zero)
      return;

    std::shared_ptr<aast::Operand> val;
    std::shared_ptr<aast::Identifier> target;
    std::unique_ptr<aast::Cond_Code> cond_code;
    if (jmp_if_zero)
    {
      val = generate_operand(std::move(jmp_if_zero->condition));
      target = std::make_shared<aast::Identifier>(jmp_if_zero->target->name);
      cond_code = std::make_unique<aast::E>();
    }
    else if (jmp_if_not_zero)
    {
      val = generate_operand(std::move(jmp_if_not_zero->condition));
      target = std::make_shared<aast::Identifier>(jmp_if_not_zero->target->name);
      cond_code = std::make_unique<aast::NE>();
    }

    std::shared_ptr<aast::Imm> imm = std::make_shared<aast::Imm>(0);
    std::unique_ptr<aast::Cmp> cmp = std::make_unique<aast::Cmp>(imm, val);
    assembly_instructions.push_back(std::move(cmp));

    std::unique_ptr<aast::JmpCC> jmp_cc = std::make_unique<aast::JmpCC>(std::move(cond_code), std::move(target));
    assembly_instructions.push_back(std::move(jmp_cc));
  }

  // This function converts a regular TACKY jump to a Assembly jump
  void generate_jmp(std::unique_ptr<tacky::Instruction> &instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Jump *jmp = dynamic_cast<tacky::Jump *>(instruction.get());
    if (!jmp)
      return;

    std::shared_ptr<aast::Identifier> identifier = std::make_shared<aast::Identifier>(jmp->target->name);
    std::unique_ptr<aast::Jmp> simple_jmp = std::make_unique<aast::Jmp>(std::move(identifier));
    assembly_instructions.push_back(std::move(simple_jmp));
  }

  // This function converts a regular TACKY label to a Assembly label
  void generate_label(std::unique_ptr<tacky::Instruction> &instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Label *label = dynamic_cast<tacky::Label *>(instruction.get());
    if (!label)
      return;

    std::shared_ptr<aast::Identifier> identifier = std::make_shared<aast::Identifier>(label->identifier->name);
    std::unique_ptr<aast::Label> a_label = std::make_unique<aast::Label>(std::move(identifier));
    assembly_instructions.push_back(std::move(a_label));
  }

  // This function converts a TACKY copy to a Mov instruction
  void generate_copy(std::unique_ptr<tacky::Instruction> &instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions)
  {
    tacky::Copy *copy = dynamic_cast<tacky::Copy *>(instruction.get());
    if (!copy)
      return;

    std::shared_ptr<aast::Operand> src = generate_operand(std::move(copy->src));
    std::shared_ptr<aast::Operand> dst = generate_operand(std::move(copy->dst));
    std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(std::move(src), std::move(dst));
    assembly_instructions.push_back(std::move(mov));
  }

  // This function converts every tacky Instruction to a set of assembly instructions
  std::list<std::unique_ptr<aast::Instruction>> generate_instructions(std::unique_ptr<tacky::Function> func)
  {
    std::list<std::unique_ptr<aast::Instruction>> assembly_instructions;
    for (std::unique_ptr<tacky::Instruction> &instruction : func->body)
    {
      generate_jmp_if(instruction, assembly_instructions);
      generate_return(instruction, assembly_instructions);
      generate_unary(instruction, assembly_instructions);
      generate_bin(instruction, assembly_instructions);
      generate_jmp(instruction, assembly_instructions);
      generate_label(instruction, assembly_instructions);
      generate_copy(instruction, assembly_instructions);
    }

    return assembly_instructions;
  }

  // This function converts the temporary variable to a offset from the base caller address within the stack frame
  std::shared_ptr<aast::Stack> replace_pseudo(aast::Pseudo *pseudo)
  {
    std::string var(pseudo->identifier->name);
    if (!stack_offset.count(var))
    { // Only aast::dd New Locations Crreated
      total_bytes_to_reserve += 4;
      stack_offset[var] = -total_bytes_to_reserve;
    }
    if (addresses.count(stack_offset[var]))
    {
      return addresses[stack_offset[var]];
    }
    addresses.insert({stack_offset[var], std::make_shared<aast::Stack>(stack_offset[var])});
    return addresses[stack_offset[var]];
  }

  // DELETE CERTAIN OBJECTS AND USE SHARED POINTERS TO RESERVE MEMORY
  // This function assists in replacing pseudo variables for mov
  void fix_mov(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions)
  {
    aast::Mov *mov = dynamic_cast<aast::Mov *>(it->get());
    if (!mov)
      return;
    aast::Pseudo *src = dynamic_cast<aast::Pseudo *>(mov->src.get());
    aast::Pseudo *dst = dynamic_cast<aast::Pseudo *>(mov->dst.get());
    std::shared_ptr<aast::Stack> src_stack, dst_stack;
    if (src && dst)
    { // separate into 2 instructions using r10d register
      src_stack = replace_pseudo(src);
      dst_stack = replace_pseudo(dst);

      std::shared_ptr<aast::R10> r10 = std::make_shared<aast::R10>();
      std::shared_ptr<aast::Reg> reg = std::make_shared<aast::Reg>(std::move(r10));
      std::unique_ptr<aast::Mov> new_mov = std::make_unique<aast::Mov>(src_stack, reg); // copies src to register

      mov->src = std::move(reg);                        // replaces current temporary variable with register
      mov->dst = dst_stack;                             // new address
      it = instructions.insert(it, std::move(new_mov)); // Inserts before the current Mov Instruction
    }
    else if (src)
    {
      src_stack = replace_pseudo(src);
      mov->src = src_stack;
    }
    else if (dst)
    {
      dst_stack = replace_pseudo(dst);
      mov->dst = dst_stack;
    }
  }

  // This function assists in replacing pseudo variables for unary operators
  void fix_unary(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it)
  {
    aast::Unary *unary = dynamic_cast<aast::Unary *>(it->get());
    if (!unary)
      return;
    aast::Pseudo *operand = dynamic_cast<aast::Pseudo *>(unary->operand.get());
    if (operand)
    {
      std::shared_ptr<aast::Stack> stack = replace_pseudo(operand);
      unary->operand = stack;
    }
  }

  // This function assists in replacing pseudo variables for addiiton and subtraction operators
  void fix_add_sub(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions)
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(it->get());
    if (!binary)
      return;
    aast::Mult *mult = dynamic_cast<aast::Mult *>(binary->binary_operator.get());
    if (mult)
      return;
    aast::Pseudo *operand1 = dynamic_cast<aast::Pseudo *>(binary->operand1.get());
    aast::Pseudo *operand2 = dynamic_cast<aast::Pseudo *>(binary->operand2.get());
    std::shared_ptr<aast::Stack> src_stack, dst_stack;
    if (operand1 && operand2)
    { // same as mov with 2 addresses
      src_stack = replace_pseudo(operand1);
      dst_stack = replace_pseudo(operand2);

      std::shared_ptr<aast::R10> r10 = std::make_shared<aast::R10>();
      std::shared_ptr<aast::Reg> reg = std::make_shared<aast::Reg>(std::move(r10));
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(src_stack, reg);

      binary->operand1 = std::move(reg);
      binary->operand2 = dst_stack;
      it = instructions.insert(it, std::move(mov));
    }
    else if (operand1)
    {
      src_stack = replace_pseudo(operand1);
      binary->operand1 = src_stack;
    }
    else if (operand2)
    {
      dst_stack = replace_pseudo(operand2);
      binary->operand2 = dst_stack;
    }

    aast::Imm *dst = dynamic_cast<aast::Imm *>(binary->operand2.get());
    if (dst)
    {
      std::shared_ptr<aast::R11> r11 = std::make_shared<aast::R11>();
      std::shared_ptr<aast::Reg> reg = std::make_shared<aast::Reg>(std::move(r11));
      binary->operand2 = reg;

      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(std::make_shared<aast::Imm>(dst), reg);
      it = instructions.insert(it, std::move(mov));
    }
  }

  // FIX THIS
  // This function assists in replacing pseudo variables for multiplication operator
  void fix_mult(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions)
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(it->get());
    if (!binary)
      return;
    aast::Mult *mult = dynamic_cast<aast::Mult *>(binary->binary_operator.get());
    if (!mult)
      return;
    aast::Pseudo *src = dynamic_cast<aast::Pseudo *>(binary->operand1.get());
    aast::Pseudo *dst = dynamic_cast<aast::Pseudo *>(binary->operand2.get());
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

  // This function assists in replacing pseudos for cmp as well as rearranging its operands
  void fix_cmp(typename std::list<aast::Instruction *>::iterator &it, std::list<aast::Instruction *> &instructions)
  {
    aast::Cmp *cmp = dynamic_cast<aast::Cmp *>(*it);
    if (!cmp)
      return;
    aast::Pseudo *operand1 = dynamic_cast<aast::Pseudo *>(cmp->operand1);
    aast::Pseudo *operand2 = dynamic_cast<aast::Pseudo *>(cmp->operand2);
    aast::Stack *src_stack = nullptr,
                *dst_stack = nullptr;
    if (operand1 && operand2)
    {
      src_stack = replace_pseudo(operand1);
      dst_stack = replace_pseudo(operand2);

      aast::R10 *r10_1 = new aast::R10(), *r10_2 = new aast::R10();
      aast::Reg *mov_reg = new aast::Reg(r10_1), *cmp_reg = new aast::Reg(r10_2);
      delete cmp->operand1;
      cmp->operand1 = cmp_reg;
      delete cmp->operand2;
      cmp->operand2 = dst_stack;

      aast::Mov *mov = new aast::Mov(src_stack, mov_reg);
      it = instructions.insert(it, mov);
    }
    else if (operand1)
    {
      src_stack = replace_pseudo(operand1);
      delete cmp->operand1;
      cmp->operand1 = src_stack;
    }
    else if (operand2)
    {
      dst_stack = replace_pseudo(operand2);
      delete cmp->operand2;
      cmp->operand2 = dst_stack;
    }

    aast::Imm *dst = dynamic_cast<aast::Imm *>(cmp->operand2);
    if (dst)
    {
      aast::R11 *r11_1 = new aast::R11(), *r11_2 = new aast::R11();
      aast::Reg *mov_reg = new aast::Reg(r11_1), *cmp_reg = new aast::Reg(r11_2);

      aast::Imm *mov_copy = new aast::Imm(dst->val);
      aast::Mov *mov = new aast::Mov(mov_copy, mov_reg);
      it = instructions.insert(it, mov);

      delete cmp->operand2;
      cmp->operand2 = cmp_reg;
    }
  }

  // This function handles setcc instructions with registers
  void fix_set(typename std::list<aast::Instruction *>::iterator &it)
  {
    aast::SetCC *setcc = dynamic_cast<aast::SetCC *>(*it);
    if (!setcc)
      return;

    aast::Pseudo *operand = dynamic_cast<aast::Pseudo *>(setcc->operand);
    if (operand)
    {
      aast::Stack *stack = replace_pseudo(operand);
      delete setcc->operand;
      setcc->operand = stack;
    }
  }

  // This function converts every temporary Variable (Pseudo) to a stack offset
  void compiler_pass(std::list<std::unique_ptr<aast::Instruction>> &instructions)
  {
    // aast::ll of the replacements follow the same steps: create aast::Stack struct, delete aast::Pseudo, replace with aast::Stack struct
    for (typename std::list<std::unique_ptr<aast::Instruction>>::iterator it = instructions.begin(); it != instructions.end(); ++it)
    {
      fix_mov(it, instructions);
      fix_unary(it);
      fix_cmp(it, instructions);
      fix_set(it);
      fix_mult(it, instructions);
      fix_add_sub(it, instructions);
      fix_div(it, instructions);
    }

    std::unique_ptr<aast::AllocateStack> allocateStack = std::make_unique<aast::AllocateStack>(total_bytes_to_reserve); // Reserves memory in stack frame for local variables
    instructions.push_front(std::move(allocateStack));
  }

  // This function converts Tacky nodes to assembly instructions
  std::unique_ptr<aast::Program> generate_top_level(std::unique_ptr<tacky::Program> tacky_program)
  {
    std::shared_ptr<aast::Identifier> identifier = std::make_shared<aast::Identifier>(tacky_program->func->identifier->name);
    std::list<std::unique_ptr<aast::Instruction>> instructions = generate_instructions(std::move(tacky_program->func));
    compiler_pass(instructions);
    std::unique_ptr<aast::Function> function = std::make_unique<aast::Function>(std::move(identifier), std::move(instructions));
    std::unique_ptr<aast::Program> program = std::make_unique<aast::Program>(std::move(function));
    return program;
  }
}
