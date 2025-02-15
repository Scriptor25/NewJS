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

    if (At("extern"))
    {
        const auto where = Skip().Where;
        if (At("function"))
            return ParseFunctionStatement(true);
        if (At("let") || At("const"))
            return ParseVariableStatement(true);
        Error(where, "the extern keyword requires either a function or variable declaration to follow");
    }

    if (At("{"))
        return ParseScopeStatement();
    if (At("for"))
        return ParseForStatement();
    if (At("function"))
        return ParseFunctionStatement(false);
    if (At("if"))
        return ParseIfStatement();
    if (At("import"))
        return ParseImportStatement();
    if (At("let") || At("const"))
        return ParseVariableStatement(false);
    if (At("return"))
        return ParseReturnStatement();
    if (At("switch"))
        return ParseSwitchStatement();
    if (At("while"))
        return ParseWhileStatement();

    return ParseExpression();
}
