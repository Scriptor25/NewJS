#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

void NJS::Parser::ParseTypeAlias()
{
    const auto where = Expect("type").Where;
    const auto name = Expect(TokenType_Symbol).String;
    Expect("=");
    m_TypeContext.GetNamedTypeReference(name) = ParseType();
}
