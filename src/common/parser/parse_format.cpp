#include <sstream>
#include <newjs/ast.hpp>
#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::ExpressionPtr NJS::Parser::ParseFormatExpression()
{
    const auto where = Expect("f").Where;
    if (!At(TokenType_String))
        return ParseSymbolExpression(where, "f");

    const auto source = Expect(TokenType_String).String;

    std::map<unsigned, std::string> static_operands;
    std::map<unsigned, ExpressionPtr> dynamic_operands;

    std::string static_operand;
    unsigned operand_index = 0;

    for (unsigned i = 0; i < source.size(); ++i)
    {
        if (source[i] != '{')
        {
            static_operand += source[i];
            continue;
        }
        if (i == source.size() - 1 || source[i + 1] == '}')
        {
            static_operand += source[i++];
            continue;
        }
        if (!static_operand.empty())
        {
            static_operands[operand_index++] = static_operand;
            static_operand.clear();
        }
        std::stringstream stream(source.substr(i + 1));
        Parser parser(
            m_TypeContext,
            m_TemplateContext,
            stream,
            SourceLocation(where.Filename, where.Row, where.Column + i + 1),
            m_MacroMap,
            m_IsMain);
        dynamic_operands[operand_index++] = parser.ParseExpression();
        i += stream.tellg();
    }
    if (!static_operand.empty())
        static_operands[operand_index++] = static_operand;

    return std::make_shared<FormatExpression>(where, operand_index, static_operands, dynamic_operands);
}
