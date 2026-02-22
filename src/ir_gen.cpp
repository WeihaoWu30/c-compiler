#include "tacky.h"
#include "ast.h"
#include <vector>
#include <string>
#include <iostream>

uint32_t counter = 0; // Assists in creating unique temporary variables

// Prototyping
TIdentifier* make_temporary();
TUnary_Operator* convert_unop(Unary_Operator* exp);
TBinary_Operator* convert_binop(Binary_Operator *op);
TVal* emit_tacky(Expression* e, std::vector<TInstruction*>& instructions);
TFunction* generate_function(Function* func);
TProgram* generate_tacky(Program* program);

// This function creates the name for a temporary variable
TIdentifier* make_temporary() {
    std::string tmp_name("tmp." + std::to_string(counter++));
    return new TIdentifier(tmp_name);
}

// This function converts from a AST Unary operator to TACKY unary operator
TUnary_Operator* convert_unop(Unary_Operator *exp) {
    Complement* complement = dynamic_cast<Complement*>(exp);
    Negate* negate = dynamic_cast<Negate*>(exp);
    if (complement){
        TComplement* t_complement = new TComplement();
        return t_complement;
    }
    else if(negate) {
        TNegate* t_negate = new TNegate();
        return t_negate;
    } else {
        return nullptr;
    }
}

// This function converts from a AST binary operator to TACKY operator
TBinary_Operator* convert_binop(Binary_Operator *op) {
    TBinary_Operator *res = nullptr;
    Add *add = dynamic_cast<Add *>(op);
    Subtract *subtract = dynamic_cast<Subtract *>(op);
    Multiply *multiply = dynamic_cast<Multiply *>(op);
    Divide *divide = dynamic_cast<Divide *>(op);
    Remainder *remainder = dynamic_cast<Remainder *>(op);
    
    if(add) {
        res = new TAdd();
    } else if(subtract) {
        res = new TSubtract();
    } else if(multiply) {
        res = new TMultiply();
    } else if(divide) {
        res = new TDivide();
    } else if(remainder) {
        res = new TRemainder();
    }
    return res;
}

// This function is recursive and converts AST expressions to Tacky Values
TVal* emit_tacky(Expression* e, std::vector<TInstruction*>& instructions) {
    Constant *constant = dynamic_cast<Constant *>(e);
    Unary *unary = dynamic_cast<Unary *>(e);
    Binary *binary = dynamic_cast<Binary *>(e);
    if (constant) {
        TConstant* t_constant = new TConstant(constant->val);
        return t_constant;
    }
    else if(unary) {
        TVal *src = emit_tacky(unary->exp, instructions);
        TIdentifier *dst_name = make_temporary();
        TVar *dst = new TVar(dst_name);
        TUnary_Operator *unary_operator = convert_unop(unary->unary_operator);
        TUnary *unary = new TUnary(unary_operator, src, dst);
        instructions.push_back(unary);
        return dst;
    } else if(binary) {
        TVal* src1 = emit_tacky(binary->left, instructions);
        TVal* src2 = emit_tacky(binary->right, instructions);
        TIdentifier *dst_name = make_temporary();
        TVar *dst = new TVar(dst_name);
        TBinary_Operator *binary_operator = convert_binop(binary->binary_operator);
        TBinary *t_binary = new TBinary(binary_operator, src1, src2, dst);
        instructions.push_back(t_binary);
        return dst;
    } 
    return nullptr;
}

// This function converts AST function to Tacky function
TFunction* generate_function(Function* func) {
    Identifier* a_identifier = func->name;
    Return* ret = dynamic_cast<Return *>(func->body);
    if(!ret) {
        std::cerr << "Failed To Cast Function Body As Return Statement" << std::endl;
        exit(1);
    }

    TIdentifier* t_identifier = new TIdentifier(a_identifier->name);
    
    std::vector<TInstruction*> instructions;
    TReturn* t_return = new TReturn(emit_tacky(ret->exp, instructions));
    instructions.push_back(t_return);
    return new TFunction(t_identifier, instructions);
}

// This function converts AST program to Tacky program
TProgram* generate_tacky(Program* program) {
    TFunction* function_definition = generate_function(program->func);
    TProgram* tacky = new TProgram(function_definition);
    return tacky;
}