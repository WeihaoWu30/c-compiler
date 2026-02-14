#ifndef __CODEGEN_H__
#define __CODEGEN_H__
#include "aast.h"
#include "ast.h"

AProgram *codegen(Program *&ast_program);
#endif // !__CODEGEN_H__
