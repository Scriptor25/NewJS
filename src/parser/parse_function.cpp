#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/TypeContext.hpp>

NJS::StatementPtr NJS::Parser::ParseFunctionStatement(const bool is_export, const bool is_extern)
{
    const auto where = Expect("function").Where;

    unsigned flags = FunctionFlags_None;
    if (is_export)
        flags |= FunctionFlags_Export;
    if (is_extern)
        flags |= FunctionFlags_Extern;

    std::vector<std::string> template_arguments;
    const auto parent_is_template = m_IsTemplate;
    const auto is_template = NextAt("<");
    if (is_template)
    {
        flags |= FunctionFlags_Template;

        m_IsTemplate = true;

        while (!At(">"))
        {
            template_arguments.push_back(Expect(TokenType_Symbol).StringValue);

            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        if (!parent_is_template)
            ResetBuffer();
    }

    std::string name;
    if (!is_extern && !is_template && NextAt("operator"))
    {
        flags |= FunctionFlags_Operator;
        name = Expect(TokenType_Operator).StringValue;
    }
    else
        name = Expect(TokenType_Symbol).StringValue;

    std::vector<ParameterPtr> parameters;
    Expect("(");
    const auto is_var_arg = ParseParameterList(parameters, ")");

    auto result_type = NextAt(":")
                           ? ParseType()
                           : m_TypeContext.GetVoidType();

    StatementPtr body;
    if (!is_extern && At("{"))
        body = ParseScopeStatement();

    if (is_template)
    {
        if (!parent_is_template)
        {
            m_IsTemplate = false;
            m_TemplateContext.InsertFunction(m_TemplateWhere, name, template_arguments, m_TemplateBuffer);
        }
        return {};
    }

    return std::make_shared<FunctionStatement>(where, flags, name, parameters, is_var_arg, result_type, body);
}

NJS::ExpressionPtr NJS::Parser::ParseFunctionExpression()
{
    const auto where = Expect("?").Where;

    std::vector<ParameterPtr> parameters;
    auto is_var_arg = false;
    if (NextAt("("))
        is_var_arg = ParseParameterList(parameters, ")");

    TypePtr result_type;
    if (NextAt(":"))
        result_type = ParseType();
    else
        result_type = m_TypeContext.GetVoidType();

    const auto body = ParseScopeStatement();

    return std::make_shared<FunctionExpression>(where, parameters, is_var_arg, result_type, body);
}
