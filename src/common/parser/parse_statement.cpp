#include <newjs/ast.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseStatement()
{
    while (NextAt(";"))
        if (AtEof())
            return nullptr;

    if (At("#"))
    {
        ParseMacro();
        return nullptr;
    }

    if (At("type"))
    {
        ParseTypeAlias();
        return nullptr;
    }

    const auto is_extern = NextAt("extern");

    if (At("break"))
        return ParseBreakStatement();
    if (At("class"))
        return ParseClassStatement();
    if (At("continue"))
        return ParseContinueStatement();
    if (At("export"))
        return ParseExportStatement();
    if (At("for"))
        return ParseForStatement();
    if (At("function"))
        return ParseFunctionStatement(is_extern);
    if (At("if"))
        return ParseIfStatement();
    if (At("import"))
        return ParseImportStatement();
    if (At("return"))
        return ParseReturnStatement();
    if (At("{"))
        return ParseScopeStatement();
    if (At("switch"))
        return ParseSwitchStatement();
    if (At("let") || At("const"))
        return ParseVariableStatement(is_extern);
    if (At("while"))
        return ParseWhileStatement();

    return ParseExpression();
}
