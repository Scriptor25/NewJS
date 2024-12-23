#include <iostream>
#include <ranges>
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
    std::vector<std::map<std::string, TypePtr>> inherit_stack)
    : m_Ctx(ctx), m_Stream(stream), m_Imported(imported), m_Stack(std::move(inherit_stack))
{
    m_Where.Filename = std::move(filename);
    m_C = m_Stream.get();
    Next();

    StackPush();

    if (m_Stack.size() == 1)
        DefVar("process") = m_Ctx.GetStructType({
            {"argc", m_Ctx.GetIntType(32, true)},
            {"argv", m_Ctx.GetPointerType(m_Ctx.GetStringType())},
        });
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

void NJS::Parser::DefOp(const std::string& sym, const TypePtr& lhs, const TypePtr& rhs, const TypePtr& result)
{
    m_BinOps[sym][lhs][rhs] = result;
}

void NJS::Parser::StackPush()
{
    m_Stack.emplace_back();
}

void NJS::Parser::StackPop()
{
    m_Stack.pop_back();
}

NJS::TypePtr& NJS::Parser::DefVar(const std::string& name)
{
    return m_Stack.back()[name];
}

NJS::TypePtr NJS::Parser::GetVar(const std::string& name)
{
    for (const auto& frame : std::ranges::reverse_view(m_Stack))
        if (frame.contains(name)) return frame.at(name);
    return m_Ctx.GetNoType();
}
