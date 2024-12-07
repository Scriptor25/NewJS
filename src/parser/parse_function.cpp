#include <NJS/AST.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::FunctionStmtPtr NJS::Parser::ParseFunction()
{
    const auto where = m_Token.Where;
    const auto extern_ = NextAt("extern");
    if (!extern_) Expect("function");
    const auto name = Expect(TokenType_Symbol).StringValue;

    std::vector<ParamPtr> params;
    Expect("(");
    const auto vararg = ParseParamList(params, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    auto& type = DefVar(name);

    StackPush();

    std::vector<TypePtr> param_types;
    for (const auto& param : params)
    {
        param->CreateVars(*this, {});
        param_types.push_back(param->Type);
    }

    type = m_Ctx.GetFunctionType(param_types, result_type, vararg);

    ScopeStmtPtr body;
    if (!extern_ && At("{"))
        body = ParseScope();

    StackPop();
    return std::make_shared<FunctionStmt>(where, extern_, name, params, vararg, result_type, body);
}
