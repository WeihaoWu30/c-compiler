#ifndef __CODEGEN_H__
#define __CODEGEN_H__
#include "aast.h"
#include "tacky.h"
#include <unordered_map>

extern std::unordered_map<std::string, int> stack_offset;
extern uint32_t total_bytes_to_reserve;

Operand *generate_operand(TVal* t_val);
AUnary_Operator *generate_operators(TUnary_Operator *unary_operator);
std::list<Instruction *> generate_instructions(TFunction* func);
Stack* replace_pseudo(Pseudo* pseudo);
void compiler_pass(std::list<Instruction *>& instructions);
AProgram *generate_top_level(TProgram *&tacky_program);
#endif // !__CODEGEN_H__
