#include <fstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::ImportStmtPtr NJS::Parser::ParseImport()
{
    const auto where = Expect("import").Where;
    const auto mapping = ParseImportMapping();
    Expect("from");
    const auto filename = Expect(TokenType_String).StringValue;
    const auto filepath = std::filesystem::path(where.Filename).parent_path() / filename;

    if (m_Imported)
        return {};

    std::ifstream stream(filepath);
    Parser parser(m_Ctx, stream, filepath.string(), true);

    std::vector<FunctionStmtPtr> functions;
    parser.Parse([&](const StmtPtr& ptr)
    {
        if (const auto function = std::dynamic_pointer_cast<FunctionStmt>(ptr))
        {
            function->Body = {};
            functions.push_back(function);
        }
    });

    mapping.MapFunctions(*this, functions);

    return std::make_shared<ImportStmt>(where, mapping, absolute(filepath), functions);
}

NJS::ImportMapping NJS::Parser::ParseImportMapping()
{
    if (At(TokenType_Symbol))
        return {Skip().StringValue, {}};

    Expect("{");
    std::map<std::string, std::string> mappings;
    while (!At("}") && !AtEof())
    {
        const auto name_ = Expect(TokenType_Symbol).StringValue;
        const auto mapping_ = NextAt(":") ? Expect(TokenType_Symbol).StringValue : name_;
        mappings[name_] = mapping_;
        if (!At("}"))
            Expect(",");
        else NextAt(",");
    }
    Expect("}");
    return {{}, std::move(mappings)};
}
