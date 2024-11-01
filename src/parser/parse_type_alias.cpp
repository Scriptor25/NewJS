#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

void NJS::Parser::ParseTypeAlias()
{
    Expect("type");
    const auto name = Expect(TokenType_Symbol).StringValue;
    Expect("=");
    m_Ctx.GetType(name) = ParseType();
}
