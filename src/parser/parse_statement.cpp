#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseStatement()
{
    while (NextAt(";"))
    {
    }

    if (At("#"))
    {
        ParseMacro();
        return {};
    }

    if (At("type"))
    {
        ParseTypeAlias();
        return {};
    }

    const auto is_export = NextAt("export");
    const auto is_extern = NextAt("extern");

    if (At("{"))
        return ParseScopeStatement();
    if (At("for"))
        return ParseForStatement();
    if (At("function"))
        return ParseFunctionStatement(is_export, is_extern);
    if (At("if"))
        return ParseIfStatement();
    if (At("import"))
        return ParseImportStatement();
    if (At("let") || At("const"))
        return ParseVariableStatement(is_extern);
    if (At("return"))
        return ParseReturnStatement();
    if (At("switch"))
        return ParseSwitchStatement();
    if (At("while"))
        return ParseWhileStatement();

    return ParseExpression();
}
