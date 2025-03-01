#include <newjs/parser.hpp>

void NJS::Parser::ParseMacro()
{
    Expect("#");
    auto name = Expect(TokenType_Symbol).String;
    std::vector<std::string> parameters;
    if (NextAt("("))
    {
        while (!At(")") && !AtEof())
        {
            parameters.emplace_back(Expect(TokenType_Symbol).String);
            if (!At(")"))
                Expect(",");
        }
        Expect(")");
    }

    auto where = CurrentLocation();
    std::string source;
    do
        source += Expect(TokenType_String).String;
    while (At(TokenType_String));
    m_MacroMap[std::move(name)] = {std::move(where), std::move(parameters), std::move(source)};
}
