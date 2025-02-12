#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseStatement()
{
    if (NextAt("#"))
    {
        const auto name = Expect(TokenType_Symbol).StringValue;
        const auto source = Expect(TokenType_String).StringValue;
        m_MacroMap[name] = {source};
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
