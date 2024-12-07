#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

void NJS::Parser::ParseTypeAlias()
{
    Expect("type");
    const auto name = Expect(TokenType_Symbol).StringValue;
    Expect("=");
    m_Ctx.GetType(name) = ParseType();
}
