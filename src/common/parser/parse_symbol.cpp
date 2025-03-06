#include <newjs/parser.hpp>
#include <newjs/template_context.hpp>

NJS::ExpressionPtr NJS::Parser::ParseSymbolExpression(const SourceLocation &where, const std::string &name)
{
    if (m_MacroMap.contains(name))
        return m_MacroMap.at(name).Inflate(*this);

    if (m_TemplateContext.HasFunction(name))
    {
        std::vector<TypePtr> arguments;

        Expect("<");
        while (!At(">") && !AtEof())
        {
            arguments.emplace_back(ParseType());

            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        std::string inflated_name;
        if (!m_IsTemplate && !m_IsImport)
            inflated_name = m_TemplateContext.InflateFunction(*this, name, arguments);
        else
            inflated_name = name;
        return std::make_shared<SymbolExpression>(where, inflated_name);
    }

    return std::make_shared<SymbolExpression>(where, name);
}
