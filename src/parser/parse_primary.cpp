#include <NJS/AST.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParsePrimaryExpr()
{
    const auto where = m_Token.Where;

    if (At(TokenType_Int))
    {
        const auto value = Skip().IntValue;
        const auto type = ParseType();
        return std::make_shared<IntExpr>(where, type, value);
    }

    if (At(TokenType_FP))
    {
        const auto value = Skip().FPValue;
        const auto type = ParseType();
        return std::make_shared<FPExpr>(where, type, value);
    }

    if (At(TokenType_String))
        return std::make_shared<StringExpr>(
            where,
            m_Ctx.GetStringType(),
            Skip().StringValue);

    if (At(TokenType_Char))
        return std::make_shared<CharExpr>(where, m_Ctx.GetIntType(8, true), Skip().StringValue[0]);

    if (NextAt("true"))
        return std::make_shared<BoolExpr>(where, m_Ctx.GetBoolType(), true);

    if (NextAt("false"))
        return std::make_shared<BoolExpr>(where, m_Ctx.GetBoolType(), false);

    if (NextAt("("))
    {
        const auto ptr = ParseExpr();
        Expect(")");
        return ptr;
    }

    if (At("{"))
        return ParseStructExpr();

    if (At("["))
        return ParseTupleExpr();

    if (At("?"))
        return ParseFunctionExpr();

    if (At("$"))
        return ParseFormatExpr();

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
        const auto operand = ParseOperandExpr();
        const auto type = operand->Type;
        return std::make_shared<UnaryExpr>(where, type, op, false, operand);
    }

    Error(m_Token.Where, "unused token {}", m_Token);
}
