#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

void NJS::Parser::ParseInterface()
{
    Expect("interface");
    const auto name = Expect(TokenType_Symbol).StringValue;
    m_Ctx.GetType(name) = ParseObjectType();
}
