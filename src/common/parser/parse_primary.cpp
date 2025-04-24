#include <newjs/ast.hpp>
#include <newjs/error.hpp>
#include <newjs/parser.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>

NJS::ExpressionPtr NJS::Parser::ParsePrimaryExpression()
{
    auto where = CurrentLocation();

    if (At(TokenType_Int))
    {
        const auto value = Skip().Int;

        TypePtr type;
        if (NextAt(":"))
            type = ParseType();

        return std::make_shared<IntegerExpression>(where, type, value);
    }

    if (At(TokenType_FP))
    {
        const auto value = Skip().Float;

        TypePtr type;
        if (NextAt(":"))
            type = ParseType();

        return std::make_shared<FloatingPointExpression>(where, type, value);
    }

    if (At(TokenType_String))
    {
        std::string value;
        do
            value += Skip().Value;
        while (At(TokenType_String));

        return std::make_shared<StringExpression>(where, value);
    }

    if (At(TokenType_Format))
    {
        return ParseFormatExpression();
    }

    if (At(TokenType_Char))
    {
        return std::make_shared<CharExpression>(where, Skip().Value[0]);
    }

    if (NextAt("true"))
    {
        return std::make_shared<BooleanExpression>(where, true);
    }

    if (NextAt("false"))
    {
        return std::make_shared<BooleanExpression>(where, false);
    }

    if (NextAt("("))
    {
        const auto ptr = ParseExpression();

        Expect(")");

        return ptr;
    }

    if (At("{"))
    {
        return ParseStructExpression();
    }

    if (At("["))
    {
        return ParseTupleExpression();
    }

    if (At("$"))
    {
        return ParseFunctionExpression();
    }

    if (At("switch"))
    {
        return ParseSwitchExpression();
    }

    if (NextAt("sizeof"))
    {
        if (NextAt("("))
        {
            const auto expr = ParseExpression();

            Expect(")");

            return std::make_shared<SizeOfExpression>(where, expr);
        }

        Expect("<");

        const auto type = ParseType();

        Expect(">");

        return std::make_shared<IntegerExpression>(
            where,
            m_TypeContext.GetIntegerType(64, false),
            type->GetSize(m_Builder));
    }

    if (NextAt("typeof"))
    {
        if (NextAt("("))
        {
            const auto expr = ParseExpression();

            Expect(")");

            return std::make_shared<TypeOfExpression>(where, expr);
        }

        Expect("<");

        const auto type = ParseType();

        Expect(">");

        return std::make_shared<StringExpression>(where, type->GetString());
    }

    if (At("asm"))
    {
        return ParseAsmExpression();
    }

    if (At(TokenType_Symbol))
    {
        return ParseSymbolExpression(where, Skip().Value);
    }

    if (At(TokenType_Operator))
    {
        const auto operator_ = Skip().Value;
        const auto operand = ParseOperandExpression();
        return std::make_shared<UnaryExpression>(where, operator_, true, operand);
    }

    Error(where, "unused token {}", m_Token);
}
