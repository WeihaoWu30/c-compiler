#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "function.h"

class Function;

class Program {
public:
    Program(Function* function_definition): func(function_definition) {}
    ~Program() { delete func; }
    friend std::ostream& operator<<(std::ostream& ostr, const Program& program);
private:
    Function* func;
};

#endif