#ifndef __FUNCTION_H__
#define __FUNCTION_H__
#include "statement.h"
#include "identifier.h"
#include <iostream>

class Identifier;
class Statement;

class Function {
   public:
      Function(Identifier* name_, Statement* body_): name(name_), body(body_) {}
      ~Function() {
         delete name;
         delete body;
      }
      friend std::ostream& operator<<(std::ostream& ostr, const Function& function);
   private:
      Identifier* name;
      Statement* body;
};

#endif