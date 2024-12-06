#include <NJS/AST.hpp>
#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Parser.hpp>

NJS::ExprPtr NJS::Parser::ParsePrimary()
{
    if (At(TokenType_Number))
        return std::make_shared<ConstNumberExpr>(Skip().NumberValue);

    if (At(TokenType_String))
        return std::make_shared<ConstStringExpr>(Skip().StringValue);

    if (At(TokenType_Char))
        return std::make_shared<ConstCharExpr>(Skip().StringValue[0]);

    if (NextAt("("))
    {
        const auto ptr = ParseExpression();
        Expect(")");
        return ptr;
    }

    if (At("{"))
        return ParseConstObject();

    if (At("["))
        return ParseConstTuple();

    if (At("?"))
        return ParseConstFunction();

    if (At("$"))
        return ParseFormat();

    if (At("switch"))
        return ParseSwitchExpr();

    if (NextAt("true"))
        return std::make_shared<ConstBooleanExpr>(true);

    if (NextAt("false"))
        return std::make_shared<ConstBooleanExpr>(false);

    if (At(TokenType_Symbol))
    {
        const auto name = Skip().StringValue;
        return std::make_shared<SymbolExpr>(name);
    }

    if (At(TokenType_Operator))
    {
        const auto op = Skip().StringValue;
        auto operand = ParseOperand();
        return std::make_shared<UnaryExpr>(op, false, operand);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
