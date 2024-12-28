#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseStmt()
{
    if (NextAt("#"))
    {
        const auto name = Expect(TokenType_Symbol).StringValue;
        const auto source = Expect(TokenType_String).StringValue;
        m_Macros[name] = {source};
        return {};
    }

    if (At("type"))
    {
        ParseTypeAlias();
        return {};
    }

    if (At("import"))
        return ParseImportStmt();

    if (At("{")) return ParseScopeStmt();
    if (At("function") || At("extern") || At("operator") || At("template")) return ParseFunctionStmt();
    if (At("let") || At("const")) return ParseVariableStmt();
    if (At("if")) return ParseIfStmt();
    if (At("for")) return ParseForStmt();
    if (At("return")) return ParseReturnStmt();
    if (At("switch")) return ParseSwitchStmt();

    return ParseExpr();
}
