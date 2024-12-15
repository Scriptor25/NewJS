#include <NJS/AST.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParseConstFunction()
{
    const auto where = Expect("?").Where;

    std::vector<ParamPtr> args;
    bool vararg = false;
    if (NextAt("("))
        vararg = ParseParamList(args, ")");

    TypePtr result_type;
    if (NextAt(": "))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    StackPush();
    std::vector<TypePtr> arg_types;
    for (const auto& arg : args)
    {
        arg->CreateVars(*this, {});
        arg_types.push_back(arg->Type);
    }
    const auto body = ParseScope();
    StackPop();

    const auto type = m_Ctx.GetFunctionType(result_type, arg_types, vararg);
    return std::make_shared<ConstFunctionExpr>(where, type, args, vararg, result_type, *body);
}
