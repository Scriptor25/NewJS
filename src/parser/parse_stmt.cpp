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
        return ParseImport();

    if (At("{")) return ParseScope();
    if (At("function") || At("extern")) return ParseFunction();
    if (At("let") || At("const")) return ParseVariable();
    if (At("if")) return ParseIf();
    if (At("for")) return ParseFor();
    if (At("return")) return ParseReturn();

    return ParseExpression();
}
