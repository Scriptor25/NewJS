#include <NJS/AST.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>

NJS::VariableStmtPtr NJS::Parser::ParseVariable()
{
    const auto is_const = NextAt("const");
    if (!is_const) Expect("let");

    const auto name = ParseParam();
    ExprPtr value;
    if ((!At("in") && !At("of") && (is_const || name->RequireValue()) && (Expect("="), true)) || NextAt("="))
        value = ParseExpression();

    return std::make_shared<VariableStmt>(is_const, name, value);
}
