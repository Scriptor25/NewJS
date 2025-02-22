#include <NJS/Error.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/TypeContext.hpp>

void NJS::Parser::ParseTypeAlias()
{
    const auto where = Expect("type").Where;

    if (m_IsTemplate)
        Error(m_Where, "cannot create type alias in template");

    std::vector<std::string> template_arguments;
    if ((m_IsTemplate = NextAt("<")))
    {
        while (!At(">") && !AtEof())
        {
            template_arguments.emplace_back(Expect(TokenType_Symbol).StringValue);

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
    else
        type = m_TypeContext.GetIncompleteType(name);

    if (m_IsTemplate)
    {
        m_IsTemplate = false;
        m_TemplateContext.InsertType(m_TemplateWhere, name, template_arguments, m_TemplateBuffer);
        return;
    }

    if (auto &ref = m_TypeContext.DefType(name); !ref || ref->IsIncomplete())
        ref = type;
}
