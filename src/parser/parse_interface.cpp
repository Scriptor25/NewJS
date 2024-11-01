#include <NJS/Context.hpp>
#include <NJS/Parser.hpp>

void NJS::Parser::ParseInterface()
{
    Expect("interface");
    const auto name = Expect(TokenType_Symbol).StringValue;
    m_Ctx.GetType(name) = ParseObjectType();
}
