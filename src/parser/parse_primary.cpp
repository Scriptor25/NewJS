#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParsePrimary()
{
    const auto where = m_Token.Where;

    if (At(TokenType_Int))
    {
        const auto value = Skip().IntValue;
        const auto type = ParseType();
        return std::make_shared<ConstIntExpr>(where, type, value);
    }

    if (At(TokenType_FP))
    {
        const auto value = Skip().FPValue;
        const auto type = ParseType();
        return std::make_shared<ConstFPExpr>(where, type, value);
    }

    if (At(TokenType_String))
        return std::make_shared<ConstStringExpr>(
            where,
            m_Ctx.GetPointerType(m_Ctx.GetIntType(8, true)),
            Skip().StringValue);

    if (At(TokenType_Char))
        return std::make_shared<ConstCharExpr>(where, m_Ctx.GetIntType(8, true), Skip().StringValue[0]);

    if (NextAt("true"))
        return std::make_shared<ConstBooleanExpr>(where, m_Ctx.GetIntType(1, false), true);

    if (NextAt("false"))
        return std::make_shared<ConstBooleanExpr>(where, m_Ctx.GetIntType(1, false), false);

    if (NextAt("("))
    {
        const auto ptr = ParseExpression();
        Expect(")");
        return ptr;
    }

    if (At("{"))
        return ParseConstStruct();

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
