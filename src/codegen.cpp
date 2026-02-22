#include "aast.h"
#include "tacky.h"
#include <iostream>
#include <list>
#include <unordered_map>

std::unordered_map<std::string, int> stack_offset; // unique rbp jumps
uint32_t total_bytes_to_reserve = 0; // total bytes to allocate to Stack Frame

// Prototyping
Operand *generate_operand(TVal *&t_val);
AUnary_Operator* generate_unary_operators(TUnary_Operator *unary_operator);
ABinary_Operator* generate_basic_binary_operators(TBinary_Operator *binary_operator);
void generate_return(TReturn *t_return, std::list<AInstruction *> &assembly_instructions);
void generate_unary(TUnary *t_unary, std::list<AInstruction *> &assembly_instructions);
void generate_bin_basic(TBinary *t_binary, std::list<AInstruction *> &assembly_instructions);
void generate_bin_div(TBinary *t_binary, std::list<AInstruction *> &assembly_instructions);
std::list<AInstruction *> generate_instructions(TFunction* func);
Stack* replace_pseudo(Pseudo* pseudo);
void fix_mov(Mov *&mov, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions);
void fix_unary(AUnary *&unary);
void fix_add_sub(ABinary *&binary, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions);
void fix_mult(ABinary *&binary, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions);
void fix_div(AIdiv *&idiv, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions);
void compiler_pass(std::list<AInstruction *>& instructions);
AProgram *generate_top_level(TProgram *&tacky_program);

// This function converts Tacky Values To Immediate Values and Temporary Variables
Operand *generate_operand(TVal *&t_val) {
  TConstant *t_constant = dynamic_cast<TConstant *>(t_val);
  TVar *t_var = dynamic_cast<TVar *>(t_val);
  if(t_constant) {
    Imm *immediate_value = new Imm(t_constant->val);
    return immediate_value;
  } else if(t_var) {
    AIdentifier *assembly_identifier = new AIdentifier(t_var->identifier->name);
    Pseudo *pseudo_identifier = new Pseudo(assembly_identifier);
    return pseudo_identifier;
  } 
  return nullptr;
}

// This function converts Tacky unary operators to Assembly instructions for unary operators
AUnary_Operator* generate_unary_operators(TUnary_Operator *unary_operator) {
   TComplement *t_complement = dynamic_cast<TComplement *>(unary_operator);
   TNegate *t_negate = dynamic_cast<TNegate *>(unary_operator);
   AUnary_Operator *res = nullptr;
   if (t_complement) {
      res = new Not();
   } else if (t_negate){
      res = new Neg();
   }
   return res;
}

// This function converts from TACKY to Assembly Instructions for Binary Operators +, -, *
ABinary_Operator* generate_basic_binary_operators(TBinary_Operator *binary_operator) {
  TAdd *t_add = dynamic_cast<TAdd *>(binary_operator);
  TSubtract *t_sub = dynamic_cast<TSubtract *>(binary_operator);
  TMultiply *t_mult = dynamic_cast<TMultiply *>(binary_operator);
  ABinary_Operator *res = nullptr;
  if(t_add) {
    res = new AAdd();
  } else if(t_sub) {
    res = new ASub();
  } else if(t_mult) {
    res = new AMult();
  }

  return res;
}

// This function creates the Assembly Return Instruction
void generate_return(TReturn *t_return, std::list<AInstruction *> &assembly_instructions) {
  Operand *val = generate_operand(t_return->val);
  AX *ax_register = new AX();
  Reg *reg = new Reg(ax_register);
  Mov *move = new Mov(val, reg);
  assembly_instructions.push_back(move);

  Ret *ret = new Ret();
  assembly_instructions.push_back(ret);
}

// This function creates the Assembly Instruction for Tacky Unary Operators
void generate_unary(TUnary *t_unary, std::list<AInstruction *> &assembly_instructions) {
  Operand *src = generate_operand(t_unary->src);
  Operand *dst = generate_operand(t_unary->dst);
  Mov *mov = new Mov(src, dst);
  assembly_instructions.push_back(mov);

  Operand *operand = generate_operand(t_unary->dst); // This is required to avoid dangling pointer later
  AUnary_Operator *unary_operator = generate_unary_operators(t_unary->unary_operator);
  AUnary *unary = new AUnary(unary_operator, operand);
  assembly_instructions.push_back(unary);
}

// This function creates the Assembly Instruction for Tacky Binary Operators +, -, *
void generate_bin_basic(TBinary *t_binary, std::list<AInstruction *> &assembly_instructions) {
  Operand *mov_src = generate_operand(t_binary->src1);
  Operand *mov_dst = generate_operand(t_binary->dst);
  Mov *mov = new Mov(mov_src, mov_dst);
  assembly_instructions.push_back(mov);

  Operand *bin_src = generate_operand(t_binary->src2);
  Operand *bin_dst = generate_operand(t_binary->dst);
  ABinary_Operator *bin_op = generate_basic_binary_operators(t_binary->binary_operator);
  ABinary *binary = new ABinary(bin_op, bin_src, bin_dst);
  assembly_instructions.push_back(binary);
}

// This function creates the Assembly Instruction for Tacky Binary Operators /, %
void generate_bin_div(TBinary *t_binary, std::list<AInstruction *> &assembly_instructions) {
  TDivide *div = dynamic_cast<TDivide *>(t_binary->binary_operator);
  TRemainder *remainder = dynamic_cast<TRemainder *>(t_binary->binary_operator);

  RegType *reg_type1 = new AX(), *reg_type2 = nullptr;
  if(div) {
    reg_type2 = new AX();  // quotient goes in eax
  }
  else if(remainder) {
    reg_type2 = new DX(); // remainder goes int edx
  }
  Reg *mov1_reg = new Reg(reg_type1);

  Operand *mov_src = generate_operand(t_binary->src1); // dividend
  Mov *mov1 = new Mov(mov_src, mov1_reg);
  assembly_instructions.push_back(mov1);
  
  ACdq *cdq = new ACdq(); // sign extension since idiv can only take in 64 bit values
  assembly_instructions.push_back(cdq);

  Operand *div_src = generate_operand(t_binary->src2); // divisor
  AIdiv *idiv = new AIdiv(div_src);
  assembly_instructions.push_back(idiv);

  Reg *mov2_reg = new Reg(reg_type2);
  Operand *mov_dst = generate_operand(t_binary->dst);
  Mov *mov2 = new Mov(mov2_reg, mov_dst); // copy register value to address
  assembly_instructions.push_back(mov2);
}

// This function converts every Tacky Instruction to a set of assembly instructions
std::list<AInstruction *> generate_instructions(TFunction* func) {
  std::list<AInstruction *> assembly_instructions;
  for(TInstruction *&instruction: func->body) {
    TReturn *t_return = dynamic_cast<TReturn *>(instruction);
    TUnary *t_unary = dynamic_cast<TUnary *>(instruction);
    TBinary *t_binary = dynamic_cast<TBinary *>(instruction);
    if(t_return) {
      generate_return(t_return, assembly_instructions);
    } else if(t_unary) {
      generate_unary(t_unary, assembly_instructions);
    } else if(t_binary) {
      TAdd *add = dynamic_cast<TAdd *>(t_binary->binary_operator);
      TSubtract *sub = dynamic_cast<TSubtract *>(t_binary->binary_operator);
      TMultiply *mult = dynamic_cast<TMultiply *>(t_binary->binary_operator);
      if(add || sub || mult) {
        generate_bin_basic(t_binary, assembly_instructions);
      } else {
        generate_bin_div(t_binary, assembly_instructions);
      }
    }
  }

  return assembly_instructions;
}

// This function converts the temporary variable to a offset from the base caller address within the stack frame
Stack* replace_pseudo(Pseudo* pseudo) {
  std::string var(pseudo->identifier->name);
  if (!stack_offset.count(var)) { // Only Add New Locations Crreated
    total_bytes_to_reserve += 4;
    stack_offset[var] = -total_bytes_to_reserve;
  } 
  Stack* stack = new Stack(stack_offset[var]); 
  return stack;
}

// This function assists in replacing pseudo variables for mov
void fix_mov(Mov *&mov, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions) {
  Pseudo *src = dynamic_cast<Pseudo *>(mov->src);
  Pseudo *dst = dynamic_cast<Pseudo *>(mov->dst);
  Stack *src_stack = nullptr, *dst_stack = nullptr;
  if (src && dst) { // separate into 2 instructions using r10d register
    src_stack = replace_pseudo(src);
    dst_stack = replace_pseudo(dst);
    
    R10 *r10_1 = new R10(), *r10_2 = new R10();
    Reg *register_1 = new Reg(r10_1), *register_2 = new Reg(r10_2);
    Mov *new_mov = new Mov(src_stack, register_1); // copies src to register

    delete mov->src;
    mov->src = register_2; // replaces current temporary variable with register
    delete mov->dst;
    mov->dst = dst_stack; // new address
    it = instructions.insert(it, new_mov); // Inserts before the current Mov Instruction
  } else if (src) {
    src_stack = replace_pseudo(src);
    delete mov->src;
    mov->src = src_stack;
  } else if (dst) {
    dst_stack = replace_pseudo(dst);
    delete mov->dst;
    mov->dst = dst_stack;
  }
}

// This function assists in replacing pseudo variables for unary operators
void fix_unary(AUnary *&unary) {
  Pseudo *operand = dynamic_cast<Pseudo *>(unary->operand);
  if(operand) {
    Stack* stack = replace_pseudo(operand);
    delete unary->operand;
    unary->operand = stack;
  }
}

// This function assists in replacing pseudo variables for addiiton and subtraction operators
void fix_add_sub(ABinary *&binary, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions) {
  Pseudo *operand1 = dynamic_cast<Pseudo *>(binary->operand1);
  Pseudo *operand2 = dynamic_cast<Pseudo *>(binary->operand2);
  Stack *src_stack = nullptr, *dst_stack = nullptr;
  if(operand1 && operand2) { // same as mov with 2 addresses
    src_stack = replace_pseudo(operand1);
    dst_stack = replace_pseudo(operand2);

    R10 *r10_1 = new R10(), *r10_2 = new R10();
    Reg *reg1 = new Reg(r10_1), *reg2 = new Reg(r10_2);
    Mov *mov = new Mov(src_stack, reg1);

    delete binary->operand1;
    binary->operand1 = reg2;
    delete binary->operand2;
    binary->operand2 = dst_stack;
    it = instructions.insert(it, mov);
  } else if(operand1) {
    src_stack = replace_pseudo(operand1);
    delete binary->operand1;
    binary->operand1 = src_stack;
  } else if(operand2) {
    dst_stack = replace_pseudo(operand2);
    delete binary->operand2;
    binary->operand2 = dst_stack;
  }
}

// This function assists in replacing pseudo variables for multiplication operator
void fix_mult(ABinary *&binary, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions) {
  Pseudo *src = dynamic_cast<Pseudo *>(binary->operand1);
  Pseudo *dst = dynamic_cast<Pseudo *>(binary->operand2);
  Stack *mov1_stack = nullptr, *mov2_stack = nullptr;
  if(src) {
    Stack *src_stack = replace_pseudo(src);
    delete binary->operand1;
    binary->operand1 = src_stack;
  }
  if(dst) {
    mov1_stack = replace_pseudo(dst);
    mov2_stack = replace_pseudo(dst);

    R11 *r11_1 = new R11(), *r11_2 = new R11(), *r11_3 = new R11();
    Reg *reg1 = new Reg(r11_1), *reg2 = new Reg(r11_2), *reg3 = new Reg(r11_3);
    Mov* mov1 = new Mov(mov1_stack, reg1); // copies address content into register
    delete binary->operand2;
    binary->operand2 = reg2; // multiplies constant by content in register
    instructions.insert(it, mov1);
    ++it;

    Mov* mov2 = new Mov(reg3, mov2_stack); // copies register content to the original address;
    it = instructions.insert(it, mov2);
  }
}

// This function assists in replacing pseudo variables for division and modulo operator
void fix_div(AIdiv *&idiv, typename std::list<AInstruction *>::iterator& it, std::list<AInstruction *>& instructions) {
  Pseudo *pseudo = dynamic_cast<Pseudo *>(idiv->operand);
  Imm *imm_val = dynamic_cast<Imm *>(idiv->operand);
  if(pseudo) {
    Stack *stack = replace_pseudo(pseudo);
    delete idiv->operand;
    idiv->operand = stack;
  } else if(imm_val) {
    R10 *r10_1 = new R10(), *r10_2 = new R10();
    Reg *reg1 = new Reg(r10_1), *reg2 = new Reg(r10_2);
    Mov *mov = new Mov(imm_val, reg1); // copies divisor into a register and then apply the division onto that register directly
    // delete idiv->operand; Mov Instruction uses its immediate value so we can't delete
    idiv->operand = reg2;
    it = instructions.insert(it, mov);
  }
}

// This function converts every Temporary Variable (Pseudo) to a stack offset
void compiler_pass(std::list<AInstruction *>& instructions) {
  // All of the replacements follow the same steps: create Stack struct, delete Pseudo, replace with Stack struct
   for(typename std::list<AInstruction *>::iterator it = instructions.begin();it != instructions.end();++it) {
      Mov *mov = dynamic_cast<Mov *>(*it);
      AUnary *unary = dynamic_cast<AUnary *>(*it);
      ABinary *binary = dynamic_cast<ABinary *>(*it);
      AIdiv *idiv = dynamic_cast<AIdiv *>(*it);
      if(mov) {
        fix_mov(mov, it, instructions);
      } else if(unary) {
        fix_unary(unary);
      } else if(binary) {
        AMult *mult = dynamic_cast<AMult *>(binary->binary_operator);
        if(mult) {
          fix_mult(binary, it, instructions);
        } else {
          fix_add_sub(binary, it, instructions);
        }
      } else if(idiv) {
        fix_div(idiv, it, instructions);
      }
   }

   AllocateStack *allocateStack = new AllocateStack(total_bytes_to_reserve); // Reserves memory in stack frame for local variables
   instructions.push_front(allocateStack);
}

// This function converts Tacky nodes to assembly AST nodes
AProgram *generate_top_level(TProgram *&tacky_program) {
   AIdentifier *identifier = new AIdentifier(tacky_program->func->identifier->name);
   std::list<AInstruction *> instructions = generate_instructions(tacky_program->func);
   compiler_pass(instructions); // Convert After We Allocate All Needed Temporary Variables
   AFunction *function = new AFunction(identifier, instructions);
   AProgram *program = new AProgram(function);
   return program;
}