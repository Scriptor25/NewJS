#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StmtPtr NJS::Parser::ParseStmt()
{
    if (At("type"))
    {
        ParseTypeAlias();
        return {};
    }

    if (At("import"))
        return ParseImportStmt();

    if (At("{")) return ParseScopeStmt();
    if (At("function") || At("extern")) return ParseFunctionStmt();
    if (At("let") || At("const")) return ParseDefStmt();
    if (At("if")) return ParseIfStmt();
    if (At("for")) return ParseForStmt();
    if (At("return")) return ParseReturnStmt();
    if (At("switch")) return ParseSwitchStmt();

    return ParseExpr();
}
