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
        TokenType_Int,
        TokenType_FP,
        TokenType_Char,
        TokenType_String,
        TokenType_Symbol,
        TokenType_Operator,
        TokenType_Other,
    };

    struct Token
    {
        SourceLocation Where;
        TokenType Type = TokenType_EOF;
        std::string StringValue;
        uint64_t IntValue = 0;
        double FPValue = 0.0;
    };
}

namespace std
{
    template<>
    struct formatter<NJS::TokenType> : formatter<string>
    {
        template<typename FormatContext>
        auto format(const NJS::TokenType type, FormatContext &ctx) const
        {
            static map<NJS::TokenType, const char *> names
            {
                {NJS::TokenType_EOF, "EOF"},
                {NJS::TokenType_Int, "Int"},
                {NJS::TokenType_FP, "FP"},
                {NJS::TokenType_Char, "Char"},
                {NJS::TokenType_String, "String"},
                {NJS::TokenType_Symbol, "Symbol"},
                {NJS::TokenType_Operator, "Operator"},
                {NJS::TokenType_Other, "Other"},
            };
            return formatter<string>::format(names[type], ctx);
        }
    };

    template<>
    struct formatter<NJS::Token> : formatter<string>
    {
        template<typename FormatContext>
        auto format(const NJS::Token &token, FormatContext &ctx) const
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
