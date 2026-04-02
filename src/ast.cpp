#include <ostream>
#include "ast/top_level/function.hpp"
#include "ast/top_level/identifier.hpp"
#include "ast/top_level/program.hpp"
#include "ast/top_level/declaration.hpp"

namespace ast
{
  std::ostream &operator<<(std::ostream &ostr, const Program &program)
  {
    ostr << "Program(" << std::endl;
    ostr << "\t" << *(program.func);
    ostr << ")" << std::endl;
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Function &function)
  {
    ostr << "Function(" << std::endl;
    ostr << "\t\tname=" << *(function.name) << ", " << std::endl;
    //  ostr << "\t\tbody=" << *(function.body) << std::endl;
    ostr << "\t)" << std::endl;
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Identifier &identifier)
  {
    ostr << "\"" << identifier.name << "\"";
    return ostr;
  }

  std::ostream &operator<<(std::ostream &ostr, const Declaration &declaration)
  {
    ostr << "Declaration(" << std::endl;
    ostr << "\t\tname=" << *declaration.name << ", " << std::endl;
    ostr << "\t\tinit=" << *declaration.init << std::endl;
    ostr << "\t)" << std::endl;
    return ostr;
  }
}
