#include <sstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

NJS::ExprPtr NJS::Parser::ParseFormatExpr()
{
    const auto where = Expect("$").Where;
    auto source = Expect(TokenType_String).StringValue;

    std::map<unsigned, std::string> statics;
    std::map<unsigned, ExprPtr> dynamics;

    unsigned index = 0;
    for (size_t beg; (beg = source.find('{')) != std::string::npos;)
    {
        if (source[beg + 1] == '{')
        {
            const auto str = source.substr(0, beg + 1);
            statics[index++] = str;
            source.erase(0, beg + 2);
            continue;
        }
        if (const auto str = source.substr(0, beg); !str.empty()) statics[index++] = str;
        source.erase(0, beg + 1);

        std::stringstream stream(source);
        Parser parser(m_TypeCtx, m_TemplateCtx, stream, SourceLocation("<dynamic>"), m_Macros);
        dynamics[index++] = parser.ParseExpr();

        source.erase(0, static_cast<unsigned>(stream.tellg()) - 1);
    }
    if (!source.empty()) statics[index++] = source;

    return std::make_shared<FormatExpr>(where, index, statics, dynamics);
}
