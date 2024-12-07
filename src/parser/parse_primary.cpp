#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParsePrimary()
{
    const auto where = m_Token.Where;

    if (At(TokenType_Number))
        return std::make_shared<ConstNumberExpr>(where, m_Ctx.GetNumberType(), Skip().NumberValue);

    if (At(TokenType_String))
        return std::make_shared<ConstStringExpr>(where, m_Ctx.GetStringType(), Skip().StringValue);

    if (At(TokenType_Char))
        return std::make_shared<ConstCharExpr>(where, m_Ctx.GetCharType(), Skip().StringValue[0]);

    if (NextAt("true"))
        return std::make_shared<ConstBooleanExpr>(where, m_Ctx.GetBooleanType(), true);

    if (NextAt("false"))
        return std::make_shared<ConstBooleanExpr>(where, m_Ctx.GetBooleanType(), false);

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

    if (At(TokenType_Symbol))
    {
        const auto name = Skip().StringValue;
        const auto type = GetVar(name);
        return std::make_shared<SymbolExpr>(where, type, name);
    }

    if (At(TokenType_Operator))
    {
        const auto op = Skip().StringValue;
        const auto operand = ParseOperand();
        const auto type = operand->Type;
        return std::make_shared<UnaryExpr>(where, type, op, false, operand);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
