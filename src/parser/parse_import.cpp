#include <fstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseImportStatement()
{
    const auto where = Expect("import").Where;
    const auto mapping = ParseImportMapping();
    Expect("from");
    const auto filename = Expect(TokenType_String).StringValue;
    const auto filepath = canonical(std::filesystem::path(where.Filename).parent_path() / filename);

    if (m_ParsedSet.contains(filepath))
        return {};

    std::ifstream stream(filepath);
    Parser parser(
        m_TypeContext,
        m_TemplateContext,
        stream,
        SourceLocation(filepath.string()),
        m_MacroMap,
        true,
        m_ParsedSet);

    std::vector<StatementPtr> functions;
    parser.Parse(
        [&](const StatementPtr &ptr)
        {
            if (m_IsImport)
                return;

            const auto function = std::dynamic_pointer_cast<FunctionStatement>(ptr);
            if (!function)
                return;

            if (function->Flags & FunctionFlags_Extern)
                return;

            function->Body = {};
            functions.push_back(function);
        });

    return std::make_shared<ImportStatement>(where, mapping, filepath, functions);
}

NJS::ImportMapping NJS::Parser::ParseImportMapping()
{
    if (NextAt("*"))
        return {true};

    if (At(TokenType_Symbol))
        return {false, Skip().StringValue, {}};

    std::string overflow;
    std::map<std::string, std::string> mappings;

    Expect("{");
    while (!At("}") && !AtEof())
    {
        if (NextAt("."))
        {
            Expect(".");
            Expect(".");
            overflow = Expect(TokenType_Symbol).StringValue;
            break;
        }

        const auto name = Expect(TokenType_Symbol).StringValue;
        const auto mapping = NextAt(":") ? Expect(TokenType_Symbol).StringValue : name;
        mappings[name] = mapping;

        if (!At("}"))
            Expect(",");
        else
            NextAt(",");
    }
    Expect("}");

    return {false, std::move(overflow), std::move(mappings)};
}
