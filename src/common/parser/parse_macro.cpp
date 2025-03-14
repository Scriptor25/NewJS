#include <newjs/parser.hpp>

using namespace std::string_view_literals;

void NJS::Parser::ParseMacro()
{
    static const std::map<std::string_view, MacroParameterType> parameter_type_map
    {
        {"stmt"sv, MacroParameterType_Statement},
        {"expr"sv, MacroParameterType_Expression},
        {"type"sv, MacroParameterType_Type},
    };

    const auto where = Expect("#").Where;
    auto name = Expect(TokenType_Symbol).String;

    std::vector<std::pair<std::string, MacroParameterType>> parameters;
    if (NextAt("("))
    {
        while (!At(")"))
        {
            auto parameter_name = Expect(TokenType_Symbol).String;
            auto parameter_types = MacroParameterType_Statement;

            if (NextAt(":"))
            {
                auto parameter_type = Expect(TokenType_Symbol).String;
                if (!parameter_type_map.contains(parameter_type))
                    Error(where, "undefined macro parameter type '{}'", parameter_type);
                parameter_types = parameter_type_map.at(parameter_type);
            }

            parameters.emplace_back(parameter_name, parameter_types);

            if (!At(")"))
                Expect(",");
        }
        Expect(")");
    }

    auto macro_where = CurrentLocation();
    std::string source;
    do
        source += Expect(TokenType_String).String;
    while (At(TokenType_String));
    m_MacroMap[std::move(name)] = {std::move(macro_where), std::move(parameters), std::move(source)};
}
