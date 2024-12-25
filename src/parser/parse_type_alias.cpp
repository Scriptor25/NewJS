#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

void NJS::Parser::ParseTypeAlias()
{
    Expect("type");
    const auto name = Expect(TokenType_Symbol).StringValue;

    TypePtr type;
    if (NextAt("="))
        type = ParseType();
    else type = m_Ctx.GetNoType();
    m_Ctx.GetType(name) = type;
}
