#include <newjs/ast.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::StatementPtr NJS::Parser::ParseFunctionStatement(const bool is_extern)
{
    const auto where = Expect("function").Where;

    unsigned flags = FunctionFlags_None;
    if (is_extern)
        flags |= FunctionFlags_Extern;

    std::string name;
    if (NextAt("operator"))
    {
        if (is_extern)
            Error(where, "operator cannot be extern");
        flags |= FunctionFlags_Operator;

        if (NextAt("("))
        {
            Expect(")");
            name = "()";
        }
        else if (NextAt("["))
        {
            Expect("]");
            name = "[]";
        }
        else
        {
            name = Expect(TokenType_Operator).String;
        }
    }
    else
        name = Expect(TokenType_Symbol).String;

    std::vector<ParameterPtr> parameters;
    Expect("(");
    const auto is_var_arg = ParseReferenceParameterList(parameters, ")");

    ReferenceInfo result;
    if (NextAt(":"))
        result = ParseReferenceInfo();
    else
        result.Type = m_TypeContext.GetVoidType();

    StatementPtr body;
    if (At("{"))
        body = ParseScopeStatement();

    return std::make_shared<FunctionStatement>(where, flags, name, parameters, is_var_arg, result, body);
}

NJS::ExpressionPtr NJS::Parser::ParseFunctionExpression()
{
    const auto where = Expect("$").Where;

    if (NextAt("["))
    {
        Expect("]");
    }

    std::vector<ParameterPtr> parameters;
    auto is_var_arg = false;
    if (NextAt("("))
        is_var_arg = ParseReferenceParameterList(parameters, ")");

    ReferenceInfo result;
    if (NextAt(":"))
        result = ParseReferenceInfo();
    else
        result.Type = m_TypeContext.GetVoidType();

    const auto body = ParseScopeStatement();

    return std::make_shared<FunctionExpression>(where, parameters, is_var_arg, result, body);
}
