#include <NJS/AST.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::FunctionStmtPtr NJS::Parser::ParseFunction()
{
    const auto where = m_Token.Where;
    const auto is_extern = NextAt("extern");
    if (!is_extern) Expect("function");
    const auto name = Expect(TokenType_Symbol).StringValue;

    std::vector<ParamPtr> args;
    Expect("(");
    const auto vararg = ParseParamList(args, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    std::vector<TypePtr> arg_types;
    for (const auto& arg : args)
        arg_types.push_back(arg->Type);
    DefVar(name) = m_Ctx.GetFunctionType(result_type, arg_types, vararg);

    ScopeStmtPtr body;
    if (!is_extern && At("{"))
    {
        StackPush();
        for (const auto& arg : args)
            arg->CreateVars(*this, {});
        body = ParseScope();
        StackPop();
    }

    return std::make_shared<FunctionStmt>(where, is_extern, name, args, vararg, result_type, body);
}
