#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ReturnStmtPtr NJS::Parser::ParseReturn()
{
    const auto where = Expect("return").Where;
    const auto value = ParseExpression();
    return std::make_shared<ReturnStmt>(where, value);
}
