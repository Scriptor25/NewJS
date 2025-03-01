#include <utility>
#include <newjs/ast.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>

NJS::Parser::Parser(
    TypeContext &type_context,
    TemplateContext &template_context,
    std::istream &stream,
    SourceLocation where,
    std::map<std::string, Macro> &macro_map,
    const bool is_main,
    const bool is_import,
    std::set<std::filesystem::path> parsed_set)
    : m_TypeContext(type_context),
      m_TemplateContext(template_context),
      m_Stream(stream),
      m_MacroMap(macro_map),
      m_IsMain(is_main),
      m_IsImport(is_import),
      m_ParsedSet(std::move(parsed_set)),
      m_Where(std::move(where))
{
    if (!m_Where.Filename.empty() && std::filesystem::exists(m_Where.Filename))
    {
        const auto filepath = std::filesystem::canonical(m_Where.Filename);
        m_Where.Filename = filepath.string();
        m_ParsedSet.insert(filepath);
    }

    Next();
}

NJS::TypeContext &NJS::Parser::GetTypeContext() const
{
    return m_TypeContext;
}

NJS::TemplateContext &NJS::Parser::GetTemplateContext() const
{
    return m_TemplateContext;
}

std::map<std::string, NJS::Macro> &NJS::Parser::GetMacroMap() const
{
    return m_MacroMap;
}

bool NJS::Parser::IsMain() const
{
    return m_IsMain;
}

bool NJS::Parser::IsImport() const
{
    return m_IsImport;
}

NJS::SourceLocation NJS::Parser::CurrentLocation() const
{
    return m_Token.Where;
}

void NJS::Parser::Parse(const Consumer &consumer)
{
    while (m_Token.Type != TokenType_EOF)
        if (const auto statement = ParseStatement())
            consumer(statement);
}

void NJS::Parser::ResetBuffer()
{
    m_TemplateBuffer.clear();
    m_TemplateWhere = m_Where;
    --m_TemplateWhere.Column;
}

int NJS::Parser::Get()
{
    m_Where.Column++;
    const auto c = m_Stream.get();
    m_TemplateBuffer.push_back(static_cast<char>(c));
    return c;
}

void NJS::Parser::UnGet()
{
    m_Where.Column--;
    m_Stream.unget();
    m_TemplateBuffer.pop_back();
}

void NJS::Parser::NewLine()
{
    m_Where.Column = 0;
    ++m_Where.Row;
}

bool NJS::Parser::AtEof() const
{
    return m_Token.Type == TokenType_EOF;
}

bool NJS::Parser::At(const TokenType type) const
{
    return m_Token.Type == type;
}

bool NJS::Parser::At(const std::string &value) const
{
    return m_Token.String == value;
}

bool NJS::Parser::NextAt(const TokenType type)
{
    if (At(type))
    {
        Next();
        return true;
    }
    return false;
}

bool NJS::Parser::NextAt(const std::string &value)
{
    if (At(value))
    {
        Next();
        return true;
    }
    return false;
}

NJS::Token NJS::Parser::Skip()
{
    auto tok = m_Token;
    Next();
    return tok;
}

NJS::Token NJS::Parser::Expect(const TokenType type)
{
    if (At(type))
        return Skip();
    Error(CurrentLocation(), "unexpected token {}, expected {}", m_Token, type);
}

NJS::Token NJS::Parser::Expect(const std::string &value)
{
    if (At(value))
        return Skip();
    Error(CurrentLocation(), "unexpected token {}, expected '{}'", m_Token, value);
}
