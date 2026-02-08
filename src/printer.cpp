#include "function.h"
#include "program.h"
#include "return.h"
#include "constant.h"
#include "identifier.h"
#include <iostream>

std::ostream& operator<<(std::ostream& ostr, const Program& program) {
    ostr << "Program(" << std::endl;
    ostr << "   " << program.func << std::endl;
    ostr << ")" << std::endl;
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Function& function) {
   ostr << "Function(" << std::endl;
   ostr << "    name=" << function.name << ", " << std::endl;
   ostr << "    body=" << function.body << std::endl;
   ostr << ")" << std::endl;
   return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Identifier& identifier) {
    ostr << "\"" << identifier.name << "\"";
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Return& ret) {
      ostr << "Return("  << std::endl;
      ostr << "   " << ret.exp << std::endl;
      ostr << ")" << std::endl;
      return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Constant& constant) {
    ostr << "Constant(" << constant.val << ")" << std::endl;
    return ostr;
}

