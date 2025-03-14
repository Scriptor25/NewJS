#include <fstream>
#include <newjs/ast.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>

NJS::StatementPtr NJS::Parser::ParseImportStatement()
{
    const auto where = Expect("import").Where;
    const auto mapping = ParseImportMapping();
    Expect("from");
    const auto filename = Expect(TokenType_String).String;

    auto filepath = std::filesystem::path(filename);
    if (filepath.is_relative())
        filepath = std::filesystem::path(where.Filename).parent_path() / filepath;
    if (!exists(filepath))
        Error(where, "failed to open import file '{}': file does not exist", filepath.string());
    filepath = canonical(filepath);

    if (m_ParsedSet.contains(filepath))
        return nullptr;

    std::ifstream stream(filepath);
    if (!stream)
        Error(where, "failed to open import file '{}'", filepath.string());

    Parser parser(
        m_TypeContext,
        m_Builder,
        stream,
        SourceLocation(filepath.string()),
        m_MacroMap,
        m_IsMain,
        true,
        m_ParsedSet);

    std::vector<ExportStatementPtr> values;
    std::set<std::string> sub_module_ids;

    try
    {
        parser.Parse(
            [&](const StatementPtr &statement)
            {
                if (const auto value = std::dynamic_pointer_cast<ImportStatement>(statement);
                    m_IsMain && value)
                {
                    for (auto &sub_module_id: value->SubModuleIDs)
                        sub_module_ids.emplace(sub_module_id);
                    sub_module_ids.emplace(value->ModuleID);
                    return;
                }

                if (auto value = std::dynamic_pointer_cast<ExportStatement>(statement))
                    values.emplace_back(value);
            });
    }
    catch (const RTError &error)
    {
        Error(error, where, "failed to parse import for file '{}'", filename);
    }

    stream.close();

    auto module_id = filepath.filename().replace_extension().string();
    if (m_IsMain)
        sub_module_ids.emplace(module_id);

    return std::make_shared<ImportStatement>(where, mapping, filepath, values, module_id, sub_module_ids);
}

NJS::ImportMapping NJS::Parser::ParseImportMapping()
{
    if (NextAt("*"))
        return {true};

    if (At(TokenType_Symbol))
        return {false, Skip().String, {}};

    std::string overflow;
    std::map<std::string, std::string> mappings;

    Expect("{");
    while (!At("}") && !AtEof())
    {
        if (NextAt("..."))
        {
            overflow = Expect(TokenType_Symbol).String;
            break;
        }

        const auto name = Expect(TokenType_Symbol).String;
        const auto mapping = NextAt(":") ? Expect(TokenType_Symbol).String : name;
        mappings[name] = mapping;

        if (!At("}"))
            Expect(",");
        else
            NextAt(",");
    }
    Expect("}");

    return {false, std::move(overflow), std::move(mappings)};
}
