#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/TypeContext.hpp>

NJS::StatementPtr NJS::Parser::ParseFunctionStatement()
{
    const auto where = m_Token.Where;

    const auto fn = NextAt("extern")
                        ? FnType_Extern
                        : NextAt("operator")
                              ? FnType_Operator
                              : NextAt("template")
                                    ? FnType_Template
                                    : (Expect("function"), FnType_Function);

    std::vector<std::string> templ_args;
    const auto parent_template = m_IsTemplate;
    if (fn == FnType_Template)
    {
        m_IsTemplate = true;

        Expect("<");
        while (!At(">") && !AtEof())
        {
            templ_args.push_back(Expect(TokenType_Symbol).StringValue);

            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        if (!parent_template)
            ResetBuffer();
    }

    const auto name = (fn == FnType_Operator ? Expect(TokenType_Operator) : Expect(TokenType_Symbol)).StringValue;

    std::vector<ParameterPtr> args;
    Expect("(");
    const auto vararg = ParseParamList(args, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else
        result_type = m_TypeContext.GetVoidType();

    StatementPtr body;
    if (fn != FnType_Extern && At("{"))
        body = ParseScopeStatement();

    if (fn == FnType_Template)
    {
        if (parent_template)
            return {};

        m_IsTemplate = false;
        m_TemplateContext.InsertFunction(name, templ_args, m_TemplateWhere, m_TemplateBuffer.str());
    }

    return std::make_shared<FunctionStatement>(where, false, fn, name, args, vararg, result_type, body);
}

NJS::ExpressionPtr NJS::Parser::ParseFunctionExpression()
{
    const auto where = Expect("?").Where;

    std::vector<ParameterPtr> args;
    bool vararg = false;
    if (NextAt("("))
        vararg = ParseParamList(args, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else
        result_type = m_TypeContext.GetVoidType();

    std::vector<TypePtr> arg_types;
    for (const auto &arg: args)
        arg_types.push_back(arg->Type);

    const auto body = ParseScopeStatement();

    return std::make_shared<FunctionExpression>(where, args, vararg, result_type, body);
}
