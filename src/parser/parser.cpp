#include <utility>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::Parser::Parser(
    TypeContext& ctx,
    std::istream& stream,
    std::string filename,
    const bool imported,
    std::set<std::filesystem::path> parsed)
    : m_Ctx(ctx), m_Stream(stream), m_Imported(imported), m_Parsed(std::move(parsed))
{
    if (!filename.empty() && std::filesystem::exists(filename))
        m_Parsed.insert(std::filesystem::canonical(filename));

    m_Where.Filename = std::move(filename);
    m_C = m_Stream.get();
    Next();
}

void NJS::Parser::Parse(const Callback& callback)
{
    while (m_Token.Type != TokenType_EOF)
        if (const auto ptr = ParseStmt())
            callback(ptr);
}

int NJS::Parser::Get()
{
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

bool NJS::Parser::At(const std::string& value) const
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

bool NJS::Parser::NextAt(const std::string& value)
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
    if (At(type)) return Skip();
    Error(m_Token.Where, "unexpected token {}, expected {}", m_Token, type);
}

NJS::Token NJS::Parser::Expect(const std::string& value)
{
    if (At(value)) return Skip();
    Error(m_Token.Where, "unexpected token {}, expected '{}'", m_Token, value);
}
