#include <newjs/ast.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>
#include <newjs/template_context.hpp>
#include <newjs/type_context.hpp>

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

        std::vector<TypePtr> types;
        while (!At(">"))
        {
            auto name = Expect(TokenType_Symbol).StringValue;
            template_arguments.emplace_back(name);
            types.emplace_back(m_TypeContext.GetIncompleteType(name));

            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        if (!parent_is_template)
            ResetBuffer();

        m_TypeContext.PushTemplate(template_arguments, types);
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
    const auto is_var_arg = ParseReferenceParameterList(parameters, ")");

    ReferenceInfo result;
    if (NextAt(":"))
        result = ParseReferenceInfo();
    else
        result.Type = m_TypeContext.GetVoidType();

    StatementPtr body;
    if (!is_extern && At("{"))
        body = ParseScopeStatement();

    if (is_template)
    {
        m_TypeContext.PopTemplate();
        if (!parent_is_template)
        {
            m_IsTemplate = false;
            m_TemplateContext.InsertFunction(m_TemplateWhere, name, template_arguments, m_TemplateBuffer);
        }
        return {};
    }

    return std::make_shared<FunctionStatement>(where, flags, name, parameters, is_var_arg, result, body);
}

NJS::ExpressionPtr NJS::Parser::ParseFunctionExpression()
{
    const auto where = Expect("$").Where;

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
