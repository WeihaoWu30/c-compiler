#ifndef __CODEGEN_H__
#define __CODEGEN_H__
#include "aast.h"
#include "tacky.h"
#include <unordered_map>

extern std::unordered_map<std::string, int> stack_offset;
extern uint32_t total_bytes_to_reserve;

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
#endif // !__CODEGEN_H__
