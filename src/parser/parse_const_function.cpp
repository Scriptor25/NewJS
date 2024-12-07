#include <NJS/AST.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParseConstFunction()
{
    const auto where = Expect("?").Where;

    std::vector<ParamPtr> params;
    bool vararg = false;
    if (NextAt("("))
        vararg = ParseParamList(params, ")");

    TypePtr result_type;
    if (NextAt(": "))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    StackPush();
    std::vector<TypePtr> param_types;
    for (const auto& param : params)
    {
        param->CreateVars(*this, {});
        param_types.push_back(param->Type);
    }
    const auto body = ParseScope();
    StackPop();

    const auto type = m_Ctx.GetFunctionType(param_types, result_type, vararg);
    return std::make_shared<ConstFunctionExpr>(where, type, params, vararg, result_type, *body);
}
