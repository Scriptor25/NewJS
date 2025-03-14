#include <utility>
#include <newjs/ast.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>

NJS::Parser::Parser(
    TypeContext &type_context,
    Builder &builder,
    std::istream &stream,
    SourceLocation where,
    std::map<std::string, Macro> &macro_map,
    const bool is_main,
    const bool is_import,
    std::set<std::filesystem::path> parsed_set)
    : m_TypeContext(type_context),
      m_Builder(builder),
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

NJS::Parser::Parser(const Parser &other, std::istream &stream, SourceLocation where)
    : Parser(
        other.m_TypeContext,
        other.m_Builder,
        stream,
        std::move(where),
        other.m_MacroMap,
        other.m_IsMain,
        other.m_IsImport,
        other.m_ParsedSet)
{
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

int NJS::Parser::Get()
{
    m_Where.Column++;
    return m_Stream.get();
}

void NJS::Parser::UnGet()
{
    m_Where.Column--;
    m_Stream.unget();
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
