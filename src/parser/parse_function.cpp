#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>

NJS::FunctionStmtPtr NJS::Parser::ParseFunction()
{
    Expect("function");
    const auto name = Expect(TokenType_Symbol).StringValue;

    std::vector<ParamPtr> params;
    Expect("(");
    ParseParamList(params, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    auto& ref = m_Ctx.CreateVar(name);

    m_Ctx.StackPush();
    std::vector<TypePtr> param_types;
    for (const auto& param : params)
    {
        param->CreateVars(m_Ctx, {});
        param_types.push_back(param->Type);
    }
    ref = m_Ctx.GetFunctionType(param_types, result_type);
    const auto body = ParseScope();
    m_Ctx.StackPop();

    return std::make_shared<FunctionStmt>(name, params, result_type, *body);
}
