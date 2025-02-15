#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>

NJS::Parser::Parser(
    TypeContext &type_context,
    TemplateContext &template_context,
    std::istream &stream,
    SourceLocation where,
    std::map<std::string, Macro> &macro_map,
    const bool is_import,
    std::set<std::filesystem::path> parsed_set)
    : m_TypeContext(type_context),
      m_TemplateContext(template_context),
      m_Stream(stream),
      m_MacroMap(macro_map),
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

    m_C = m_Stream.get();
    Next();
}

void NJS::Parser::Parse(const Consumer &consumer)
{
    while (m_Token.Type != TokenType_EOF)
        if (const auto ptr = ParseStatement())
            consumer(ptr);
}

void NJS::Parser::ResetBuffer()
{
    m_TemplateBuffer.str({});
    m_TemplateBuffer.clear();
    m_TemplateWhere = m_Where;
    --m_TemplateWhere.Col;
}

int NJS::Parser::Get()
{
    m_TemplateBuffer.put(static_cast<char>(m_C));

    ++m_Where.Col;
    return m_C = m_Stream.get();
}

void NJS::Parser::NewLine()
{
    m_Where.Col = 0;
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
    return m_Token.StringValue == value;
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
    Error(m_Token.Where, "unexpected token {}, expected {}", m_Token, type);
}

NJS::Token NJS::Parser::Expect(const std::string &value)
{
    if (At(value))
        return Skip();
    Error(m_Token.Where, "unexpected token {}, expected '{}'", m_Token, value);
}
