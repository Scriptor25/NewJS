#include <newjs/parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseAsmExpression()
{
    auto where = Expect("asm").Where;

    std::set<std::string> attributes;
    while (!At("("))
        attributes.insert(Expect(TokenType_Symbol).String);

    auto is_volatile = attributes.contains("volatile");
    auto is_align = attributes.contains("align");
    auto is_throw = attributes.contains("throw");
    auto is_intel = attributes.contains("intel");

    Expect("(");

    auto source = Expect(TokenType_String).String;

    std::string constraints;
    TypePtr output_type;
    if (NextAt(":") && !At(":"))
    {
        constraints = Expect(TokenType_String).String;
        Expect("<");
        output_type = ParseType();
        Expect(">");
    }

    std::vector<std::pair<std::string, ExpressionPtr>> inputs;
    if (NextAt(":"))
    {
        do
        {
            auto input_constraint = Expect(TokenType_String).String;
            Expect("(");
            auto input_operand = ParseExpression();
            Expect(")");
            inputs.emplace_back(input_constraint, input_operand);
        }
        while (NextAt(","));
    }

    Expect(")");

    std::vector<ExpressionPtr> operands;
    for (unsigned i = 0; i < inputs.size(); ++i)
    {
        if (i > 0 || !constraints.empty())
            constraints += ',';
        constraints += inputs[i].first;
        operands.emplace_back(inputs[i].second);
    }

    return std::make_shared<AsmExpression>(
        std::move(where),
        std::move(source),
        std::move(constraints),
        is_intel,
        is_volatile,
        is_align,
        is_throw,
        std::move(operands),
        std::move(output_type));
}
