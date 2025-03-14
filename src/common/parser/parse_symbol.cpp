#include <newjs/parser.hpp>

NJS::ExpressionPtr NJS::Parser::ParseSymbolExpression(const SourceLocation &where, const std::string &name)
{
    if (m_MacroMap.contains(name))
        return m_MacroMap.at(name).Inflate(*this);

    return std::make_shared<SymbolExpression>(where, name);
}
