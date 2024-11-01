#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ReturnStmtPtr NJS::Parser::ParseReturn()
{
    Expect("return");
    const auto value = ParseExpression();
    return std::make_shared<ReturnStmt>(value);
}
