#include <NJS/AST.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseDefStmt()
{
    const auto where = m_Token.Where;
    const auto is_const = NextAt("const");
    if (!is_const) Expect("let");

    const auto name = ParseParam();
    ExprPtr value;
    if ((!At("in") && !At("of") && (is_const || name->RequireValue()) && (Expect("="), true)) || NextAt("="))
        value = ParseExpr();

    return std::make_shared<VariableStmt>(where, is_const, name, value);
}
