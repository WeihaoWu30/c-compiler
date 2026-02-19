#include "tacky.h"
#include "ast.h"
#include <vector>
#include <string>
#include <iostream>

uint32_t COUNTER = 0;

TIdentifier* make_temporary() {
    return new TIdentifier("tmp." + std::to_string(COUNTER++));
}

TUnary_Operator* convert_unop(Unary_Operator* exp) {
    Complement* complement = dynamic_cast<Complement*>(exp);
    Negate* negate = dynamic_cast<Negate*>(exp);
    if (complement){
        TComplement* t_complement = new TComplement();
        return t_complement;
    }
    else if(negate) {
        TNegate* t_negate = new TNegate();
        return t_negate;
    }
    return nullptr;
}

TVal* emit_tacky(Expression* e, std::vector<TInstruction*>& instructions) {
    Constant* constant = dynamic_cast<Constant*>(e);
    Unary* unary = dynamic_cast<Unary*>(e);
    if (constant) {
        TConstant* t_constant = new TConstant(constant->val);
        return t_constant;
    }
    else if(unary) {
        TVal* src = emit_tacky(unary->exp, instructions);
        TIdentifier* dst_name = make_temporary();
        TVar* dst = new TVar(dst_name);
        TUnary_Operator* unary_operator = convert_unop(unary->unary_operator);
        instructions.push_back(new TUnary(unary_operator, src, dst));
        return dst;
    } 
    return nullptr;
}

TFunction* generate_function(Function* func) {
    Identifier* a_identifier = func->name;
    Return* ret = dynamic_cast<Return *>(func->body);
    if(!ret) {
        std::cerr << "Failed To Cast Function Body As Return Statement" << std::endl;
        exit(1);
    }

    TIdentifier* t_identifier = new TIdentifier(a_identifier->name);
    
    std::vector<TInstruction*> instructions;
    TVar* dst = dynamic_cast<TVar *>(emit_tacky(ret->exp, instructions));
    TReturn* t_return = new TReturn(dst);
    instructions.push_back(t_return);
    return new TFunction(t_identifier, instructions);
}

TProgram* generate_tacky(Program* program) {
    TFunction* function_definition = generate_function(program->func);
    TProgram* tacky = new TProgram(function_definition);
    return tacky;
}