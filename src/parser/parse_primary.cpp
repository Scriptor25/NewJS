#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

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

    if (At("?"))
        return ParseFunctionExpression();

    if (At("$"))
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
        return std::make_shared<IntegerExpression>(where, m_TypeContext.GetIntegerType(64, false), type->GetSize());
    }

    if (NextAt("typeof"))
    {
        Expect("(");
        const auto expr = ParseExpression();
        Expect(")");
        return std::make_shared<TypeOfExpression>(where, expr);
    }

    if (At(TokenType_Symbol))
    {
        const auto name = Skip().StringValue;

        if (m_MacroMap.contains(name))
            return m_MacroMap.at(name).Inflate(*this);

        if (m_TemplateContext.HasFunction(name))
        {
            std::vector<TypePtr> arguments;

            Expect("<");
            while (!At(">") && !AtEof())
            {
                arguments.emplace_back(ParseType());

                if (!At(">"))
                    Expect(",");
            }
            Expect(">");

            std::string inflated_name;
            if (!m_IsTemplate)
                inflated_name = m_TemplateContext.InflateFunction(*this, name, arguments);
            else
                inflated_name = name;
            return std::make_shared<SymbolExpression>(where, inflated_name);
        }

        return std::make_shared<SymbolExpression>(where, name);
    }

    if (At(TokenType_Operator))
    {
        const auto op = Skip().StringValue;
        const auto operand = ParseOperandExpression();
        return std::make_shared<UnaryExpression>(where, op, true, operand);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
