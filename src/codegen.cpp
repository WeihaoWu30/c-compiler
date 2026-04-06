#include "aast/aast.hpp"
#include "tacky/tacky.hpp"
#include "compiler/codegen.hpp"
#include <ostream>
#include <list>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>

namespace codegen
{
  std::unordered_map<std::string, int> stack_offset; // unique rbp jumps
  std::unordered_map<int, aast::Stack *> stack_objs;
  uint32_t total_bytes_to_reserve = 0; // total bytes to allocate to aast::Stack Frame

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
      std::unique_ptr<aast::Pseudo> pseudo_identifier = std::make_unique<aast::Pseudo>(assembly_identifier);
      operands.push_back(std::move(pseudo_identifier));
      return operands.back().get();
    }
    return nullptr;
  }

  // This function converts Tacky unary operators to assembly instructions for unary operators
  aast::Unary_Operator *generate_unary_operators(tacky::Unary_Operator *unary_operator)
  {
    tacky::Complement *t_complement = dynamic_cast<tacky::Complement *>(unary_operator);
    tacky::Negate *t_negate = dynamic_cast<tacky::Negate *>(unary_operator);
    tacky::Not *t_not = dynamic_cast<tacky::Not *>(unary_operator);
    aast::Unary_Operator *res = nullptr;
    if (t_complement)
    {
      res = new aast::Not();
    }
    else if (t_negate)
    {
      res = new aast::Neg();
    }
    else if (t_not)
    {
      res = new aast::Not();
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

  // This function converts the relational operators into the assembly code
  aast::Cond_Code *generate_conditional_codes(tacky::Binary_Operator *binary_operator)
  {
    tacky::Equal *eq = dynamic_cast<tacky::Equal *>(binary_operator);
    tacky::NotEqual *neq = dynamic_cast<tacky::NotEqual *>(binary_operator);
    tacky::LessThan *lt = dynamic_cast<tacky::LessThan *>(binary_operator);
    tacky::LessOrEqual *lte = dynamic_cast<tacky::LessOrEqual *>(binary_operator);
    tacky::GreaterThan *gt = dynamic_cast<tacky::GreaterThan *>(binary_operator);
    tacky::GreaterOrEqual *gte = dynamic_cast<tacky::GreaterOrEqual *>(binary_operator);
    aast::Cond_Code *res = nullptr;
    if (eq)
      res = new aast::E();
    else if (neq)
      res = new aast::NE();
    else if (lt)
      res = new aast::L();
    else if (lte)
      res = new aast::LE();
    else if (gt)
      res = new aast::G();
    else if (gte)
      res = new aast::GE();

    return res;
  }

  // This function creates the assembly Return Instruction
  void generate_return(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Return *t_return = dynamic_cast<tacky::Return *>(instruction);
    if (!t_return)
      return;
    aast::Operand *val = generate_operand(t_return->val, operands);
    aast::AX *ax_register = new aast::AX();
    std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(ax_register);
    std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(val, reg.get());
    assembly_instructions.push_back(std::move(mov));

    std::unique_ptr<aast::Ret> ret = std::unique_ptr<aast::Ret>();
    assembly_instructions.push_back(std::move(ret));

    operands.push_back(std::move(reg));
  }

  // This function creates the assembly Instruction for tacky Unary Operators
  void generate_unary(tacky::Instruction *instruction, std::list<std::unique_ptr<aast::Instruction>> &assembly_instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    tacky::Unary *t_unary = dynamic_cast<tacky::Unary *>(instruction);
    if (!t_unary)
      return;
    tacky::Not *t_not = dynamic_cast<tacky::Not *>(t_unary->unary_operator);
    if (t_not)
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
    tacky::Add *add = dynamic_cast<tacky::Add *>(t_binary->binary_operator);
    tacky::Subtract *subt = dynamic_cast<tacky::Subtract *>(t_binary->binary_operator);
    tacky::Multiply *mult = dynamic_cast<tacky::Multiply *>(t_binary->binary_operator);
    tacky::Divide *div = dynamic_cast<tacky::Divide *>(t_binary->binary_operator);
    tacky::Remainder *remainder = dynamic_cast<tacky::Remainder *>(t_binary->binary_operator);
    tacky::Equal *eq = dynamic_cast<tacky::Equal *>(t_binary->binary_operator);
    tacky::NotEqual *neq = dynamic_cast<tacky::NotEqual *>(t_binary->binary_operator);
    tacky::LessThan *lt = dynamic_cast<tacky::LessThan *>(t_binary->binary_operator);
    tacky::LessOrEqual *lte = dynamic_cast<tacky::LessOrEqual *>(t_binary->binary_operator);
    tacky::GreaterThan *gt = dynamic_cast<tacky::GreaterThan *>(t_binary->binary_operator);
    tacky::GreaterOrEqual *gte = dynamic_cast<tacky::GreaterOrEqual *>(t_binary->binary_operator);
    if (add || subt || mult)
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
    else if (div || remainder)
    {
      aast::RegType *reg_type1 = new aast::AX(), *reg_type2 = nullptr;
      if (div)
      {
        reg_type2 = new aast::AX(); // quotient goes in eax
      }
      else if (remainder)
      {
        reg_type2 = new aast::DX(); // remainder goes int edx
      }
      std::unique_ptr<aast::Reg> mov_reg1 = std::make_unique<aast::Reg>(reg_type1);

      aast::Operand *mov_src = generate_operand(t_binary->src1, operands); // dividend
      std::unique_ptr<aast::Mov> mov1 = std::make_unique<aast::Mov>(mov_src, mov_reg1.get());
      assembly_instructions.push_back(std::move(mov1));

      std::unique_ptr<aast::Cdq> cdq = std::make_unique<aast::Cdq>(); // sign extension since idiv can only take in 6 bit values
      assembly_instructions.push_back(std::move(cdq));

      aast::Operand *div_src = generate_operand(t_binary->src2, operands); // divisor
      std::unique_ptr<aast::Idiv> idiv = std::make_unique<aast::Idiv>(div_src);
      assembly_instructions.push_back(std::move(idiv));

      std::unique_ptr<aast::Reg> mov_reg2 = std::make_unique<aast::Reg>(reg_type2);
      aast::Operand *mov_dst = generate_operand(t_binary->dst, operands);
      std::unique_ptr<aast::Mov> mov2 = std::make_unique<aast::Mov>(mov_reg2.get(), mov_dst); // copy register value to address
      assembly_instructions.push_back(std::move(mov2));
      operands.push_back(std::move(mov_reg1));
      operands.push_back(std::move(mov_reg2));
    }
    else if (eq || neq || lt || lte || gt || gte)
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

  // This function converts every tacky Instruction to a set of assembly instructions
  void generate_instructions(tacky::Function *func, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    for (std::unique_ptr<tacky::Instruction> &instruction : func->body)
    {
      generate_jmp_if(instruction.get(), instructions, operands);
      generate_return(instruction.get(), instructions, operands);
      generate_unary(instruction.get(), instructions, operands);
      generate_bin(instruction.get(), instructions, operands);
      generate_jmp(instruction.get(), instructions);
      generate_label(instruction.get(), instructions);
      generate_copy(instruction.get(), instructions, operands);
    }
  }

  // This function converts the temporary variable to a offset from the base caller address within the stack frame
  aast::Stack *replace_pseudo(aast::Pseudo *pseudo, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    std::string var(pseudo->identifier->name);
    if (!stack_offset.count(var))
    { // Only add New Locations Crreated
      total_bytes_to_reserve += 4;
      stack_offset[var] = -total_bytes_to_reserve;
      std::unique_ptr<aast::Stack> stack = std::make_unique<aast::Stack>(stack_offset[var]);
      stack_objs.insert({stack_offset[var], stack.get()});
      operands.push_back(std::move(stack));
    }
    return stack_objs[stack_offset[var]];
  }

  // This function assists in replacing pseudo variables for mov
  void fix_mov(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    aast::Mov *mov = dynamic_cast<aast::Mov *>(it->get());
    if (!mov)
      return;
    aast::Pseudo *src = dynamic_cast<aast::Pseudo *>(mov->src);
    aast::Pseudo *dst = dynamic_cast<aast::Pseudo *>(mov->dst);
    aast::Stack *src_stack = nullptr, *dst_stack = nullptr;
    if (src && dst)
    { // separate into 2 instructions using r10d register
      src_stack = replace_pseudo(src, operands);
      dst_stack = replace_pseudo(dst, operands);

      aast::R10 *r10 = new aast::R10();
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(r10);
      std::unique_ptr<aast::Mov> new_mov = std::make_unique<aast::Mov>(src_stack, reg.get()); // copies src to register

      // delete mov->src;
      mov->src = reg.get(); // replaces current temporary variable with register
      // delete mov->dst;
      mov->dst = dst_stack;                             // new address
      it = instructions.insert(it, std::move(new_mov)); // Inserts before the current Mov Instruction
      operands.push_back(std::move(reg));
    }
    else if (src)
    {
      src_stack = replace_pseudo(src, operands);
      // delete mov->src;
      mov->src = src_stack;
    }
    else if (dst)
    {
      dst_stack = replace_pseudo(dst, operands);
      // delete mov->dst;
      mov->dst = dst_stack;
    }
  }

  // This function assists in replacing pseudo variables for unary operators
  void fix_unary(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    aast::Unary *unary = dynamic_cast<aast::Unary *>(it->get());
    if (!unary)
      return;
    aast::Pseudo *operand = dynamic_cast<aast::Pseudo *>(unary->operand);
    if (operand)
    {
      aast::Stack *stack = replace_pseudo(operand, operands);
      // delete unary->operand;
      unary->operand = stack;
    }
  }

  // This function assists in replacing pseudo variables for addiiton and subtraction operators
  void fix_add_sub(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(it->get());
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
      src_stack = replace_pseudo(operand1, operands);
      dst_stack = replace_pseudo(operand2, operands);

      aast::R10 *r10 = new aast::R10();
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(r10);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(src_stack, reg.get());

      // delete binary->operand1;
      binary->operand1 = reg.get();
      // delete binary->operand2;
      binary->operand2 = dst_stack;
      it = instructions.insert(it, std::move(mov));
      operands.push_back(std::move(reg));
    }
    else if (operand1)
    {
      src_stack = replace_pseudo(operand1, operands);
      // delete binary->operand1;
      binary->operand1 = src_stack;
    }
    else if (operand2)
    {
      dst_stack = replace_pseudo(operand2, operands);
      // delete binary->operand2;
      binary->operand2 = dst_stack;
    }

    aast::Imm *dst = dynamic_cast<aast::Imm *>(binary->operand2);
    if (dst)
    {
      aast::R11 *r11 = new aast::R11();
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(r11);
      // delete binary->operand2;
      binary->operand2 = reg.get();

      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(dst, reg.get());
      it = instructions.insert(it, std::move(mov));

      operands.push_back(std::move(reg));
    }
  }

  // This function assists in replacing pseudo variables for multiplication operator
  void fix_mult(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    aast::Binary *binary = dynamic_cast<aast::Binary *>(it->get());
    if (!binary)
      return;
    aast::Mult *mult = dynamic_cast<aast::Mult *>(binary->binary_operator);
    if (!mult)
      return;
    aast::Pseudo *src = dynamic_cast<aast::Pseudo *>(binary->operand1);
    aast::Pseudo *dst = dynamic_cast<aast::Pseudo *>(binary->operand2);
    if (src)
    {
      aast::Stack *src_stack = replace_pseudo(src, operands);
      // delete binary->operand1;
      binary->operand1 = src_stack;
    }
    if (dst)
    {
      aast::Stack *dst_stack = replace_pseudo(dst, operands);

      aast::R11 *r11 = new aast::R11();
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(r11);
      std::unique_ptr<aast::Mov> mov1 = std::make_unique<aast::Mov>(dst_stack, reg.get()); // copies address content into register
      // delete binary->operand2;
      binary->operand2 = reg.get(); // multiplies constant by content in register
      instructions.insert(it, std::move(mov1));
      ++it;

      std::unique_ptr<aast::Mov> mov2 = std::make_unique<aast::Mov>(reg.get(), dst_stack); // copies register content to the original address;
      it = instructions.insert(it, std::move(mov2));

      operands.push_back(std::move(reg));
    }
  }

  // This function assists in replacing pseudo variables for division and modulo operator
  void fix_div(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    aast::Idiv *idiv = dynamic_cast<aast::Idiv *>(it->get());
    if (!idiv)
      return;
    aast::Pseudo *pseudo = dynamic_cast<aast::Pseudo *>(idiv->operand);
    aast::Imm *imm_val = dynamic_cast<aast::Imm *>(idiv->operand);
    if (pseudo)
    {
      aast::Stack *stack = replace_pseudo(pseudo, operands);
      // delete idiv->operand;
      idiv->operand = stack;
    }
    else if (imm_val)
    {
      aast::R10 *r10 = new aast::R10();
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(r10);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(imm_val, reg.get()); // copies divisor into a register and then apply the division onto that register directly
      // delete idiv->operand; aast::Mov Instruction uses its immediate value so we can't delete
      idiv->operand = reg.get();
      it = instructions.insert(it, std::move(mov));
      operands.push_back(std::move(reg));
    }
  }

  // This function assists in replacing pseudos for cmp as well as rearranging its operands
  void fix_cmp(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    aast::Cmp *cmp = dynamic_cast<aast::Cmp *>(it->get());
    if (!cmp)
      return;
    aast::Pseudo *operand1 = dynamic_cast<aast::Pseudo *>(cmp->operand1);
    aast::Pseudo *operand2 = dynamic_cast<aast::Pseudo *>(cmp->operand2);
    aast::Stack *src_stack = nullptr,
                *dst_stack = nullptr;
    if (operand1 && operand2)
    {
      src_stack = replace_pseudo(operand1, operands);
      dst_stack = replace_pseudo(operand2, operands);

      aast::R10 *r10 = new aast::R10();
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(r10);
      // delete cmp->operand1;
      cmp->operand1 = reg.get();
      // delete cmp->operand2;
      cmp->operand2 = dst_stack;

      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(src_stack, reg.get());
      it = instructions.insert(it, std::move(mov));

      operands.push_back(std::move(reg));
    }
    else if (operand1)
    {
      src_stack = replace_pseudo(operand1, operands);
      // delete cmp->operand1;
      cmp->operand1 = src_stack;
    }
    else if (operand2)
    {
      dst_stack = replace_pseudo(operand2, operands);
      // delete cmp->operand2;
      cmp->operand2 = dst_stack;
    }

    aast::Imm *dst = dynamic_cast<aast::Imm *>(cmp->operand2);
    if (dst)
    {
      aast::R11 *r11 = new aast::R11();
      std::unique_ptr<aast::Reg> reg = std::make_unique<aast::Reg>(r11);

      std::unique_ptr<aast::Imm> mov_copy = std::make_unique<aast::Imm>(dst->val);
      std::unique_ptr<aast::Mov> mov = std::make_unique<aast::Mov>(mov_copy.get(), reg.get());
      it = instructions.insert(it, std::move(mov));

      // delete cmp->operand2;
      cmp->operand2 = reg.get();

      operands.push_back(std::move(reg));
      operands.push_back(std::move(mov_copy));
    }
  }

  // This function handles setcc instructions with registers
  void fix_set(typename std::list<std::unique_ptr<aast::Instruction>>::iterator &it, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    aast::SetCC *setcc = dynamic_cast<aast::SetCC *>(it->get());
    if (!setcc)
      return;

    aast::Pseudo *operand = dynamic_cast<aast::Pseudo *>(setcc->operand);
    if (operand)
    {
      aast::Stack *stack = replace_pseudo(operand, operands);
      // delete setcc->operand;
      setcc->operand = stack;
    }
  }

  // This function converts every temporary Variable (Pseudo) to a stack offset
  void compiler_pass(std::list<std::unique_ptr<aast::Instruction>> &instructions, std::vector<std::unique_ptr<aast::Operand>> &operands)
  {
    // aast::ll of the replacements follow the same steps: create aast::Stack struct, delete aast::Pseudo, replace with aast::Stack struct
    for (typename std::list<std::unique_ptr<aast::Instruction>>::iterator it = instructions.begin(); it != instructions.end(); ++it)
    {
      fix_mov(it, instructions, operands);
      fix_unary(it, operands);
      fix_cmp(it, instructions, operands);
      fix_set(it, operands);
      fix_mult(it, instructions, operands);
      fix_add_sub(it, instructions, operands);
      fix_div(it, instructions, operands);
    }

    std::unique_ptr<aast::AllocateStack> allocateStack = std::make_unique<aast::AllocateStack>(total_bytes_to_reserve); // Reserves memory in stack frame for local variables
    instructions.push_front(std::move(allocateStack));
  }

  // This function converts Tacky nodes to assembly instructions
  aast::Program *generate_top_level(tacky::Program *&tacky_program)
  {
    aast::Identifier *identifier = new aast::Identifier(tacky_program->func->identifier->name);
    std::vector<std::unique_ptr<aast::Operand>> operands;
    std::list<std::unique_ptr<aast::Instruction>> instructions;
    generate_instructions(tacky_program->func, instructions, operands);
    compiler_pass(instructions, operands); // Convert aast::fter We aast::llocate aast::ll Needed tacky::emporary Variables
    aast::Function *function = new aast::Function(identifier, std::move(instructions), std::move(operands));
    aast::Program *program = new aast::Program(function);
    return program;
  }
}
