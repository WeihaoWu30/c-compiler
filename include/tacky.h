#ifndef __TACKY_H__
#define __TACKY_H__
#include <iostream>
#include <string>

struct TInstruction
{
    virtual ~TInstruction() = default;
};

struct TVal
{
    virtual ~TVal() = default;
};

struct TUnary_Operator
{
   virtual ~TUnary_Operator() = default;
};

struct TBinary_Operator {
    virtual ~TBinary_Operator() = default;
};

struct TIdentifier
{
    std::string name;
    TIdentifier(std::string name_) : name(name_) {}
};

struct TComplement : TUnary_Operator {};

struct TNegate : TUnary_Operator {};

struct TAdd : TBinary_Operator {};

struct TSubtract : TBinary_Operator {};

struct TMultiply : TBinary_Operator {};

struct TDivide : TBinary_Operator {};

struct TRemainder : TBinary_Operator {};

struct TConstant : TVal {
    int val;
    TConstant(int val_): val(val_) {}
};

struct TVar : TVal {
   TIdentifier* identifier;
   TVar(TIdentifier* identifier_): identifier(identifier_) {}
    ~TVar() { delete identifier; }
};

struct TReturn : TInstruction
{
    TVal *val;
    TReturn(TVal *val_): val(val_) {} 
    ~TReturn() { 
        delete val; 
    }
};

struct TUnary : TInstruction
{
    TUnary_Operator *unary_operator;
    TVal *src, *dst;
    TUnary(TUnary_Operator *unary_operator_, TVal *src_, TVal *dst_): unary_operator(unary_operator_), src(src_), dst(dst_) {}
    ~TUnary() {
        delete unary_operator;
        delete src; 
        // delete dst; 
        /*
        The return struct contains the final dst variable so we only need to 
        delete sources since it will clear each dst for us in the upcoming
        instructions starting from the first instruction
        */
    }
};

struct TBinary : TInstruction {
    TBinary_Operator *binary_operator;
    TVal *src1, *src2, *dst;
    TBinary(TBinary_Operator *binary_operator_, TVal *src1_, TVal *src2_, TVal *dst_): binary_operator(binary_operator_), src1(src1_), src2(src2_), dst(dst_) {} 
    ~TBinary() {
        delete binary_operator;
        delete src1;
        delete src2;
        // delete dst; Same reasoning as TUnary
    }
};

struct TFunction
{
    TIdentifier *identifier;
    std::vector<TInstruction *> body;
    TFunction(TIdentifier *identifer_, std::vector<TInstruction *>& body_) : identifier(identifer_), body(body_) {}
    ~TFunction() {
        delete identifier;
        for(TInstruction*& instr: body) delete instr;
    }
};

struct TProgram
{
    TFunction *func;
    TProgram(TFunction *function_definition) : func(function_definition) {}
    ~TProgram() { delete func; }
};

#endif
