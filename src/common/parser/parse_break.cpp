#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseBreakStatement()
{
    auto where = Expect("break").Where;
    return std::make_shared<BreakStatement>(where);
}
