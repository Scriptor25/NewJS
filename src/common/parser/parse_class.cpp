#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::StatementPtr NJS::Parser::ParseClassStatement()
{
    auto where = Expect("class").Where;
    auto class_name = Expect(TokenType_Symbol).Value;

    const auto class_type = m_TypeContext.GetStructType(class_name);
    m_TypeContext.GetNamedTypeReference(class_name) = class_type;

    if (!NextAt("{"))
        return std::make_shared<ClassStatement>(where, class_name);

    std::vector<StructElement> elements;
    std::vector<ExpressionPtr> functions;

    while (!At("}"))
    {
        auto is_const = NextAt("const");
        const auto is_reference = NextAt("&");
        auto member_name = Expect(TokenType_Symbol).Value;

        TypePtr member_type;
        ExpressionPtr default_value;

        if (!is_const && !is_reference && At("("))
        {
            auto function_where = Skip().Where;

            std::vector<ParameterPtr> parameters;
            const auto is_var_arg = ParseReferenceParameterList(parameters, ")", true);

            ReferenceInfo result;
            if (NextAt(":"))
                result = ParseReferenceInfo();
            else
                result.Type = m_TypeContext.GetVoidType();

            auto body = ParseScopeStatement();

            is_const = true;
            member_type = m_TypeContext.GetFunctionType(result, parameters, is_var_arg);
            default_value = std::make_shared<FunctionCacheExpression>(
                function_where,
                "class." + class_name + '.' + member_name,
                parameters,
                is_var_arg,
                result,
                m_IsImport ? nullptr : body);
            functions.emplace_back(default_value);
        }
        else
        {
            Expect(":");
            member_type = ParseType();
            if ((!is_reference && is_const && (Expect("="), true)) || NextAt("="))
                default_value = ParseExpression();
            if (!is_reference && is_const)
                default_value = std::make_shared<CacheExpression>(default_value->Where, default_value);
        }

        elements.emplace_back(member_name, ReferenceInfo(member_type, is_const, is_reference), default_value);

        if (!At("}"))
            Expect(",");
    }
    Expect("}");

    class_type->SetElements(elements);

    return std::make_shared<ClassStatement>(where, class_name, functions);
}
