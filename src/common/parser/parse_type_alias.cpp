#include <newjs/error.hpp>
#include <newjs/parser.hpp>
#include <newjs/template_context.hpp>
#include <newjs/type_context.hpp>

void NJS::Parser::ParseTypeAlias()
{
    const auto where = Expect("type").Where;

    if (m_IsTemplate)
        Error(m_Where, "cannot create type alias in template");

    std::vector<std::string> template_arguments;
    if ((m_IsTemplate = NextAt("<")))
    {
        std::vector<TypePtr> types;
        while (!At(">") && !AtEof())
        {
            auto name = Expect(TokenType_Symbol).String;
            template_arguments.emplace_back(name);
            types.emplace_back(m_TypeContext.GetIncompleteType(name));

            if (!At(">"))
                Expect(",");
        }
        Expect(">");

        m_TypeContext.PushTemplate(template_arguments, types);
    }

    const auto name = Expect(TokenType_Symbol).String;
    if (m_IsTemplate)
        ResetBuffer();

    TypePtr type;
    if ((m_IsTemplate && (Expect("="), true)) || NextAt("="))
        type = ParseType();
    else
        type = m_TypeContext.GetIncompleteType(name);

    if (m_IsTemplate)
    {
        m_TypeContext.PopTemplate();
        m_IsTemplate = false;
        m_TemplateContext.InsertType(m_TemplateWhere, name, template_arguments, m_TemplateBuffer);
        return;
    }

    if (auto &ref = m_TypeContext.DefType(name); !ref || ref->IsIncomplete())
        ref = type;
}
