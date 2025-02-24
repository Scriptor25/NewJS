#include <NJS/Parser.hpp>

void NJS::Parser::ParseMacro()
{
    Expect("#");
    auto name = Expect(TokenType_Symbol).StringValue;
    std::vector<std::string> parameters;
    if (NextAt("("))
    {
        while (!At(")") && !AtEof())
        {
            parameters.emplace_back(Expect(TokenType_Symbol).StringValue);
            if (!At(")"))
                Expect(",");
        }
        Expect(")");
    }

    std::string source;
    do
        source += Expect(TokenType_String).StringValue;
    while (At(TokenType_String));
    m_MacroMap[std::move(name)] = {std::move(parameters), std::move(source)};
}
