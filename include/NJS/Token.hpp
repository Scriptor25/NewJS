#pragma once

#include <format>
#include <map>
#include <string>
#include <NJS/SourceLocation.hpp>

namespace NJS
{
    enum TokenType
    {
        TokenType_EOF,
        TokenType_Number,
        TokenType_Operator,
        TokenType_Other,
        TokenType_Parenthesis,
        TokenType_String,
        TokenType_Char,
        TokenType_Symbol,
    };

    struct Token
    {
        SourceLocation Where;
        TokenType Type = TokenType_EOF;
        std::string StringValue;
        double NumberValue = 0.0;
    };
}

namespace std
{
    template <>
    struct formatter<NJS::TokenType> : formatter<string>
    {
        template <typename FormatContext>
        auto format(const NJS::TokenType type, FormatContext& ctx) const
        {
            static map<NJS::TokenType, const char*> names
            {
                {NJS::TokenType_EOF, "EOF"},
                {NJS::TokenType_Number, "Number"},
                {NJS::TokenType_Operator, "Operator"},
                {NJS::TokenType_Other, "Other"},
                {NJS::TokenType_Parenthesis, "Parenthesis"},
                {NJS::TokenType_String, "String"},
                {NJS::TokenType_Symbol, "Symbol"},
            };
            return formatter<string>::format(names[type], ctx);
        }
    };

    template <>
    struct formatter<NJS::Token> : formatter<string>
    {
        template <typename FormatContext>
        auto format(const NJS::Token& token, FormatContext& ctx) const
        {
            return format_to(
                ctx.out(),
                "[ {}: '{}' ]",
                token.Type,
                token.StringValue
            );
        }
    };
}
