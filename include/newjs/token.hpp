#pragma once

#include <format>
#include <map>
#include <string>
#include <newjs/location.hpp>

namespace NJS
{
    enum TokenType
    {
        TokenType_EOF,
        TokenType_Int,
        TokenType_FP,
        TokenType_Char,
        TokenType_String,
        TokenType_Format,
        TokenType_Symbol,
        TokenType_Operator,
        TokenType_Other,
    };

    struct Token
    {
        SourceLocation Where;
        TokenType Type = TokenType_EOF;
        std::string Raw;
        std::string Value;
        uint64_t Int = 0;
        double Float = 0.0;
    };
}

namespace std
{
    template<>
    struct formatter<NJS::TokenType> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const NJS::TokenType type, FormatContext &ctx) const
        {
            static const map<NJS::TokenType, const char *> names
            {
                {NJS::TokenType_EOF, "EOF"},
                {NJS::TokenType_Int, "Int"},
                {NJS::TokenType_FP, "FP"},
                {NJS::TokenType_Char, "Char"},
                {NJS::TokenType_String, "String"},
                {NJS::TokenType_Format, "Format"},
                {NJS::TokenType_Symbol, "Symbol"},
                {NJS::TokenType_Operator, "Operator"},
                {NJS::TokenType_Other, "Other"},
            };
            return formatter<string>::format(names.contains(type) ? names.at(type) : "?", ctx);
        }
    };

    template<>
    struct formatter<NJS::Token> final : formatter<string>
    {
        template<typename FormatContext>
        auto format(const NJS::Token &token, FormatContext &ctx) const
        {
            return format_to(
                ctx.out(),
                "[ {}: '{}' ]",
                token.Type,
                token.Value
            );
        }
    };
}
