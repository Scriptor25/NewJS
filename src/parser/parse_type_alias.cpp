#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>

#include "NJS/TemplateContext.hpp"

void NJS::Parser::ParseTypeAlias()
{
    const auto where = Expect("type").Where;

    if (m_IsTemplate)
        Error(m_Where, "cannot create type alias in template");

    std::vector<std::string> templ_args;
    if ((m_IsTemplate = NextAt("<")))
    {
        while (!At(">") && !AtEof())
        {
            templ_args.push_back(Expect(TokenType_Symbol).StringValue);

            if (!At(">"))
                Expect(",");
        }
        Expect(">");
    }

    const auto name = Expect(TokenType_Symbol).StringValue;
    if (m_IsTemplate)
        ResetBuffer();

    TypePtr type;
    if ((m_IsTemplate && (Expect("="), true)) || NextAt("="))
        type = ParseType();
    else type = m_TypeCtx.GetNoType(name);

    if (m_IsTemplate)
    {
        m_IsTemplate = false;
        m_TemplateCtx.InsertType(name, templ_args, m_TemplateWhere, m_TemplateBuffer.str());
        return;
    }

    m_TypeCtx.GetType(name) = type;
}
