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

TLabel* make_label(){

}

TConstant* convert_short_circuiting_ops(TVal* src1, TVal* src2){

}

// This function converts from a AST Unary operator to TACKY unary operator
TUnary_Operator* convert_unop(Unary_Operator *exp) {
    Complement* complement = dynamic_cast<Complement*>(exp);
    Negate* negate = dynamic_cast<Negate*>(exp);
    Unary_Not* not_op = dynamic_cast<Unary_Not*>(exp);
    if (complement){
        TComplement* t_complement = new TComplement();
        return t_complement;
    }
    else if(negate) {
        TNegate* t_negate = new TNegate();
        return t_negate;
    } else if(not_op){
      TNot* t_not = new TNot();
      return t_not;
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
    Equal *equal = dynamic_cast<Equal *>(op);
    NotEqual *not_equal = dynamic_cast<NotEqual *>(op);
    LessThan *less_than = dynamic_cast<LessThan *>(op);
    LessOrEqual *less_or_equal = dynamic_cast<LessOrEqual *>(op);
    GreaterThan *greater_than = dynamic_cast<GreaterThan *>(op);
    GreaterOrEqual *greater_or_equal = dynamic_cast<GreaterOrEqual *>(op);
    And* and_op = dynamic_cast<And *>(op);
    Or* or_op = dynamic_cast<Or *>(op);

    
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
    } else if(equal){
       res = new TEqual();
    } else if (not_equal){
       res = new TNotEqual();
    } else if (less_than){
       res = new TLessThan();
    } else if (less_or_equal){
       res = new TLessOrEqual();
    } else if (greater_than){
       res = new TGreaterOrEqual();
    } else if (greater_or_equal){
       res = new TGreaterOrEqual();
    } else if (and_op){

    } else if (or_op){

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

        And* and_op = dynamic_cast<And *>(binary->binary_operator);
        Or* or_op = dynamic_cast<Or *>(binary->binary_operator);
        if (and_op){
         TConstant* result;
         if (!src1){
            TConstant* short_circuit = new TConstant(0);
            TIdentifier* false_label = make_temporary();   //will need to edit function to make labels that are easy to differentiate from the tmps
            TJumpIfZero* jump_if_zero = new TJumpIfZero(src1, false_label);
            TCopy* copy = new TCopy(short_circuit, result);
            if (!src2){
               TConstant* short_circuit = new TConstant(0);
               TIdentifier* false_label = make_temporary();   //will need to edit function to make labels that are easy to differentiate from the tmps
               TJumpIfZero* jump_if_zero = new TJumpIfZero(src2, false_label);
               TCopy* copy = new TCopy(short_circuit, result);
            } else{
               TConstant* result = new TConstant(0);
               TBinary *binary = new TBinary(binary_operator, src1, src2, result);
               instructions.push_back(binary);
            }
         }
        } else if (or_op){

        }else{
            TBinary *binary = new TBinary(binary_operator, src1, src2, dst);
            instructions.push_back(binary);
            return dst;
        }
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