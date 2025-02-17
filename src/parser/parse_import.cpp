#include <fstream>
#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>

NJS::StatementPtr NJS::Parser::ParseImportStatement()
{
    const auto where = Expect("import").Where;
    const auto mapping = ParseImportMapping();
    Expect("from");
    const auto filename = Expect(TokenType_String).StringValue;

    if (m_IsImport && !m_IsMain)
        return {};

    auto filepath = std::filesystem::path(filename);
    if (filepath.is_relative())
        filepath = std::filesystem::path(where.Filename).parent_path() / filepath;
    if (!exists(filepath))
        Error(where, "failed to open import file '{}': file does not exist", filepath.string());
    filepath = canonical(filepath);

    if (m_ParsedSet.contains(filepath))
        return {};

    std::ifstream stream(filepath);
    if (!stream)
        Error(where, "failed to open import file '{}'", filepath.string());

    Parser parser(
        m_TypeContext,
        m_TemplateContext,
        stream,
        SourceLocation(filepath.string()),
        m_MacroMap,
        m_IsMain,
        true,
        m_ParsedSet);

    std::vector<StatementPtr> functions;
    std::set<std::string> sub_module_ids;

    parser.Parse(
        [&](const StatementPtr &ptr)
        {
            if (const auto import_ = std::dynamic_pointer_cast<ImportStatement>(ptr))
            {
                for (auto &sub_module_id: import_->SubModuleIDs)
                    sub_module_ids.emplace(sub_module_id);
                sub_module_ids.emplace(import_->ModuleID);
                return;
            }

            if (m_IsImport)
                return;

            if (auto function = std::dynamic_pointer_cast<FunctionStatement>(ptr);
                function && !(function->Flags & FunctionFlags_Extern))
            {
                function->Body = {};
                functions.emplace_back(function);
                return;
            }
        });

    stream.close();

    auto module_id = filepath.filename().replace_extension().string();
    if (m_IsMain)
        sub_module_ids.emplace(module_id);

    return std::make_shared<ImportStatement>(where, mapping, filepath, functions, module_id, sub_module_ids);
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
