#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParseConstFunction()
{
    Expect("?");

    std::vector<ParamPtr> params;
    bool vararg = false;
    if (NextAt("("))
        vararg = ParseParamList(params, ")");

    TypePtr result_type;
    if (NextAt(": "))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    const auto body = ParseScope();
    return std::make_shared<ConstFunctionExpr>(params, vararg, result_type, *body);
}
