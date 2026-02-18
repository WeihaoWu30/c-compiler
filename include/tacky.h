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

struct TIdentifier
{
    std::string name;
    TIdentifier(std::string name_) : name(name_) {}
};

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
    TVal *src;
    TVal *dst;
    TUnary(TUnary_Operator *unary_operator_, TVal *src_, TVal *dst_): unary_operator(unary_operator_), src(src_), dst(dst_) {}
    ~TUnary() {
        delete unary_operator;
        delete src; 
    }
};

struct TComplement : TUnary_Operator {};

struct TNegate : TUnary_Operator {};

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
