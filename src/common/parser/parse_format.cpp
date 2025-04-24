#include <sstream>
#include <llvm/IR/InlineAsm.h>
#include <newjs/ast.hpp>
#include <newjs/parser.hpp>
#include <newjs/type_context.hpp>

NJS::ExpressionPtr NJS::Parser::ParseFormatExpression()
{
    const auto [
        where_,
        type_,
        raw_,
        value_,
        int_,
        float_
    ] = Expect(TokenType_Format);

    std::vector<FormatNodePtr> nodes;

    auto source = value_;
    for (size_t pos; (pos = source.find("${")) != std::string::npos;)
    {
        if (pos != 0)
        {
            auto value = source.substr(0, pos);
            nodes.emplace_back(std::make_unique<ConstantFormatNode>(std::move(value)));
        }

        source = source.substr(pos + 2);

        std::stringstream stream(source);
        Parser parser(
            m_TypeContext,
            m_Builder,
            stream,
            where_,
            m_MacroMap,
            m_IsMain);
        auto value = parser.ParseExpression();
        nodes.emplace_back(std::make_unique<ExpressionFormatNode>(std::move(value)));

        source = source.substr(stream.gcount());
        source = source.substr(source.find('}') + 1);
    }

    if (!source.empty())
    {
        nodes.emplace_back(std::make_unique<ConstantFormatNode>(std::move(source)));
    }

    return std::make_shared<FormatExpression>(where_, std::move(nodes));
}
