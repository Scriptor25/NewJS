#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseContinueStatement()
{
    auto where = Expect("continue").Where;
    return std::make_shared<ContinueStatement>(where);
}
