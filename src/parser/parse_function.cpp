#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::StmtPtr NJS::Parser::ParseFunctionStmt()
{
    const auto where = m_Token.Where;

    FnType fn = FnType_Function;
    const auto is_extern = NextAt("extern");
    if (is_extern)
        fn = FnType_Extern;
    else if (NextAt("operator"))
        fn = FnType_Operator;
    else Expect("function");

    const auto name = (fn == FnType_Operator ? Expect(TokenType_Operator) : Expect(TokenType_Symbol)).StringValue;

    std::vector<ParamPtr> args;
    Expect("(");
    const auto vararg = ParseParamList(args, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else result_type = m_Ctx.GetVoidType();

    StmtPtr body;
    if (!is_extern && At("{"))
        body = ParseScopeStmt();

    return std::make_shared<FunctionStmt>(where, fn, name, args, vararg, result_type, body);
}

NJS::ExprPtr NJS::Parser::ParseFunctionExpr()
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

    std::vector<TypePtr> arg_types;
    for (const auto& arg : args)
        arg_types.push_back(arg->Type);

    const auto body = ParseScopeStmt();

    return std::make_shared<FunctionExpr>(where, args, vararg, result_type, body);
}
