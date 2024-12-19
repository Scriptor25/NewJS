#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseReturnStmt()
{
    const auto where = Expect("return").Where;
    const auto value = ParseExpr();
    return std::make_shared<ReturnStmt>(where, value);
}
