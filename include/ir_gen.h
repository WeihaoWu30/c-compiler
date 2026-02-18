#ifndef __IR_GEN_H__
#define __IR_GEN_H__
#include "tacky.h"
#include <vector>
#include "ast.h"

TIdentifier* make_temporary();
TUnary_Operator* convert_unop(Unary_Operator* exp);
TVal* emit_tacky(Expression* e, std::vector<TInstruction*>& instructions);
TFunction* generate_function(Function* func);
TProgram* generate_tacky(Program* program);

#endif