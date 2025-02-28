#include <newjs/ast.hpp>
#include <newjs/error.hpp>
#include <newjs/parser.hpp>
#include <newjs/template_context.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>

NJS::ExpressionPtr NJS::Parser::ParsePrimaryExpression()
{
    const auto where = m_Token.Where;

    if (At(TokenType_Int))
    {
        const auto value = Skip().IntValue;
        TypePtr type;
        if (NextAt(":"))
            type = ParseType();
        return std::make_shared<IntegerExpression>(where, type, value);
    }

    if (At(TokenType_FP))
    {
        const auto value = Skip().FPValue;
        TypePtr type;
        if (NextAt(":"))
            type = ParseType();
        return std::make_shared<FloatingPointExpression>(where, type, value);
    }

    if (At(TokenType_String))
    {
        std::string value;
        do
            value += Skip().StringValue;
        while (At(TokenType_String));
        return std::make_shared<StringExpression>(where, value);
    }

    if (At(TokenType_Char))
        return std::make_shared<CharExpression>(where, Skip().StringValue[0]);

    if (NextAt("true"))
        return std::make_shared<BooleanExpression>(where, true);

    if (NextAt("false"))
        return std::make_shared<BooleanExpression>(where, false);

    if (NextAt("("))
    {
        const auto ptr = ParseExpression();
        Expect(")");
        return ptr;
    }

    if (At("{"))
        return ParseStructExpression();

    if (At("["))
        return ParseTupleExpression();

    if (At("$"))
        return ParseFunctionExpression();

    if (At("f"))
        return ParseFormatExpression();

    if (At("switch"))
        return ParseSwitchExpression();

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
            type->GetSize(m_TemplateContext.GetBuilder()));
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

    if (At(TokenType_Symbol))
        return ParseSymbolExpression(where, Skip().StringValue);

    if (At(TokenType_Operator))
    {
        const auto op = Skip().StringValue;
        const auto operand = ParseOperandExpression();
        return std::make_shared<UnaryExpression>(where, op, true, operand);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
