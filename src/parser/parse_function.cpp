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

    std::vector<TypePtr> arg_types;
    for (const auto& arg : args)
        arg_types.push_back(arg->Type);

    const auto type = m_Ctx.GetFunctionType(result_type, arg_types, vararg);
    if (fn == FnType_Operator)
    {
        if (vararg)
            Error(where, "operator must have a fixed number of args");
        if (result_type->IsVoid())
            Error(where, "operator must return something");
        if (arg_types.size() == 2)
            DefOp(name, arg_types[0], arg_types[1], result_type);
        else Error(where, "operator must have 2 args");
    }
    else DefVar(name) = type;

    StmtPtr body;
    if (!is_extern && At("{"))
    {
        StackPush();
        for (const auto& arg : args)
            arg->CreateVars(*this, {});
        body = ParseScopeStmt();
        StackPop();
    }

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

    StackPush();
    std::vector<TypePtr> arg_types;
    for (const auto& arg : args)
    {
        arg->CreateVars(*this, {});
        arg_types.push_back(arg->Type);
    }
    const auto body = ParseScopeStmt();
    StackPop();

    const auto type = m_Ctx.GetFunctionType(result_type, arg_types, vararg);
    return std::make_shared<FunctionExpr>(where, type, args, vararg, result_type, body);
}
