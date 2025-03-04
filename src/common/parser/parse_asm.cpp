#include <newjs/parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseAsmExpression()
{
    auto where = Expect("asm").Where;

    Expect("<");

    auto asm_string = Expect(TokenType_String).String;

    std::string dialect;
    if (NextAt(":"))
        dialect = Expect(TokenType_Symbol).String;

    std::string constraints;
    if (NextAt(":"))
        constraints = Expect(TokenType_String).String;

    std::set<std::string> attributes;
    while (NextAt(":"))
        attributes.insert(Expect(TokenType_Symbol).String);

    Expect(">");

    std::vector<ExpressionPtr> arguments;
    if (NextAt("("))
    {
        while (!At(")"))
        {
            arguments.emplace_back(ParseExpression());
            if (!At(")"))
                Expect(",");
        }
        Expect(")");
    }

    const auto has_side_effects = attributes.contains("sideeffect");
    const auto is_align_stack = attributes.contains("alignstack");
    const auto can_throw = attributes.contains("throw");

    return std::make_shared<AsmExpression>(
        std::move(where),
        std::move(asm_string),
        std::move(constraints),
        std::move(dialect),
        has_side_effects,
        is_align_stack,
        can_throw,
        std::move(arguments));
}
