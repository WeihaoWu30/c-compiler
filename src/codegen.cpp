#include "aast.h"
#include "tacky.h"
#include <iostream>
#include <list>
#include <unordered_map>

std::unordered_map<std::string, int> stack_offset; // unique rbp jumps
uint32_t total_bytes_to_reserve = 0; // total bytes to allocate to Stack Frame

// Prototyping
Operand *generate_operand(TVal *&t_val);
AUnary_Operator *generate_operators(TUnary_Operator *unary_operator);
std::list<Instruction *> generate_instructions(TFunction* func);
Stack* replace_pseudo(Pseudo* pseudo);
void compiler_pass(std::list<Instruction *>& instructions);
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

// This function converts Tacky operators to Assembly instructions for operators
AUnary_Operator *generate_operators(TUnary_Operator *unary_operator) {
   TComplement *t_complement = dynamic_cast<TComplement *>(unary_operator);
   TNegate *t_negate = dynamic_cast<TNegate *>(unary_operator);
   if (t_complement){
      Not* not_operator = new Not();
      return not_operator;
   } else if (t_negate){
      Neg* neg_operator = new Neg();
      return neg_operator;
   }
   return nullptr;
}

// This function converts every Tacky Instruction to a set of assembly instructions
std::list<Instruction *> generate_instructions(TFunction* func) {
  std::list<Instruction *> assembly_instructions;
  for(TInstruction *&instruction: func->body) {
    TReturn *t_return = dynamic_cast<TReturn *>(instruction);
    TUnary *t_unary = dynamic_cast<TUnary *>(instruction);
    if(t_return) {
      Operand *val = generate_operand(t_return->val);
      AX *ax_register = new AX();
      Reg *reg = new Reg(ax_register);
      Mov *move = new Mov(val, reg);
      assembly_instructions.push_back(move);

      Ret *ret = new Ret();
      assembly_instructions.push_back(ret);
    } else if(t_unary) {
      Operand *src = generate_operand(t_unary->src);
      Operand *dst = generate_operand(t_unary->dst);
      Mov *mov = new Mov(src, dst);
      assembly_instructions.push_back(mov);

      Operand *operand = generate_operand(t_unary->dst); // This is required to avoid dangling pointer later
      AUnary_Operator *unary_operator = generate_operators(t_unary->unary_operator);
      AUnary *unary = new AUnary(unary_operator, operand);
      assembly_instructions.push_back(unary);
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

// This function converts every Temporary Variable (Pseudo) to a stack offset
void compiler_pass(std::list<Instruction *>& instructions) {
  // All of the replacements follow the same steps: create Stack struct, delete Pseudo, replace with Stack struct
   for(typename std::list<Instruction *>::iterator it = instructions.begin();it != instructions.end();++it) {
      Mov *mov = dynamic_cast<Mov *>(*it);
      AUnary *unary = dynamic_cast<AUnary *>(*it);
      if(mov) {
        Pseudo *src = dynamic_cast<Pseudo *>(mov->src);
        Pseudo *dst = dynamic_cast<Pseudo *>(mov->dst);
        Stack *src_stack, *dst_stack;
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
          Stack *src_stack = replace_pseudo(src);
          delete mov->src;
          mov->src = src_stack;
        } else if (dst) {
          Stack *dst_stack = replace_pseudo(dst);
          delete mov->dst;
          mov->dst = dst_stack;
        }
      } else if(unary) {
        Pseudo *operand = dynamic_cast<Pseudo *>(unary->operand);
        if(operand) {
          Stack* stack = replace_pseudo(operand);
          delete unary->operand;
          unary->operand = stack;
        }
      }
   }

   AllocateStack *allocateStack = new AllocateStack(total_bytes_to_reserve); // Reserves memory in stack frame for local variables
   instructions.push_front(allocateStack);
}

// This function converts Tacky nodes to assembly AST nodes
AProgram *generate_top_level(TProgram *&tacky_program) {
   AIdentifier *identifier = new AIdentifier(tacky_program->func->identifier->name);
   std::list<Instruction *> instructions = generate_instructions(tacky_program->func);
   compiler_pass(instructions); // Convert After We Allocate All Needed Temporary Variables
   AFunction *function = new AFunction(identifier, instructions);
   AProgram *program = new AProgram(function);
   return program;
}