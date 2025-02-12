#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseStatement()
{
    if (NextAt("#"))
    {
        auto name = Expect(TokenType_Symbol).StringValue;
        std::vector<std::string> parameters;
        if (NextAt("("))
        {
            while (!At(")") && !AtEof())
            {
                parameters.push_back(Expect(TokenType_Symbol).StringValue);
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
        return {};
    }

    if (At("type"))
    {
        ParseTypeAlias();
        return {};
    }

    if (At("import"))
        return ParseImportStatement();

    if (At("{"))
        return ParseScopeStatement();
    if (At("function") || At("extern") || At("operator") || At("template"))
        return ParseFunctionStatement();
    if (At("let") || At("const"))
        return ParseVariableStatement();
    if (At("if"))
        return ParseIfStatement();
    if (At("for"))
        return ParseForStatement();
    if (At("return"))
        return ParseReturnStatement();
    if (At("switch"))
        return ParseSwitchStatement();

    return ParseExpression();
}
