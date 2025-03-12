#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::StatementPtr NJS::Parser::ParseClassStatement()
{
    auto where = Expect("class").Where;
    auto class_name = Expect(TokenType_Symbol).String;

    GetTypeContext().DefType(class_name) = GetTypeContext().GetIncompleteType(class_name);

    if (!NextAt("{"))
        return std::make_shared<ClassStatement>(where, class_name);

    std::vector<std::pair<std::string, ReferenceInfo>> elements;
    while (!At("}"))
    {
        auto is_const = NextAt("const");
        const auto is_reference = NextAt("&");
        auto member_name = Expect(TokenType_Symbol).String;

        TypePtr member_type;
        if (!is_const && !is_reference && NextAt("("))
        {
            std::vector<ParameterPtr> parameters;
            const auto is_var_arg = ParseReferenceParameterList(parameters, ")");

            ReferenceInfo result;
            if (NextAt(":"))
                result = ParseReferenceInfo();
            else
                result.Type = GetTypeContext().GetVoidType();

            StatementPtr body;
            if (At("{"))
                body = ParseScopeStatement();

            member_type = GetTypeContext().GetFunctionType(result, parameters, is_var_arg);
            is_const = true;
        }
        else
        {
            Expect(":");
            member_type = ParseType();
        }

        elements.emplace_back(member_name, ReferenceInfo(member_type, is_const, is_reference));

        if (!At("}"))
            Expect(",");
    }
    Expect("}");

    GetTypeContext().DefType(class_name) = GetTypeContext().GetStructType(elements);

    return std::make_shared<ClassStatement>(where, class_name);
}
