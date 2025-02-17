#include <NJS/Parser.hpp>
#include <NJS/Template.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

std::string NJS::Template::GetName(const std::vector<TypePtr> &arguments) const
{
    std::string arguments_string;
    for (unsigned i = 0; i < arguments.size(); ++i)
    {
        if (i > 0)
            arguments_string += ", ";
        arguments_string += arguments[i]->GetString();
    }
    return Name + '<' + arguments_string + '>';
}

NJS::FunctionStatement NJS::Template::InflateFunction(Parser &parent, const std::vector<TypePtr> &arguments) const
{
    parent.GetTypeContext().PushTemplate(Parameters, arguments);

    std::stringstream stream('?' + Source, std::ios_base::in);
    Parser parser(
        parent.GetTypeContext(),
        parent.GetTemplateContext(),
        stream,
        Where,
        parent.GetMacroMap(),
        parent.IsMain());
    auto inflated = std::dynamic_pointer_cast<FunctionExpression>(parser.ParseFunctionExpression());

    parent.GetTypeContext().PopTemplate();

    return {
        std::move(inflated->Where),
        FunctionFlags_Absolute,
        GetName(arguments),
        std::move(inflated->Parameters),
        inflated->IsVarArg,
        inflated->ResultType,
        inflated->Body,
    };
}

NJS::TypePtr NJS::Template::InflateType(Parser &parent, const std::vector<TypePtr> &arguments) const
{
    parent.GetTypeContext().PushTemplate(Parameters, arguments);

    std::stringstream stream(Source, std::ios_base::in);
    Parser parser(
        parent.GetTypeContext(),
        parent.GetTemplateContext(),
        stream,
        Where,
        parent.GetMacroMap(),
        parent.IsMain());
    auto inflated = parser.ParseType();

    parent.GetTypeContext().PopTemplate();

    return inflated;
}
