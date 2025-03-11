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

    auto &ref = m_IsTemplate
                    ? *static_cast<TypePtr *>(nullptr)
                    : m_TypeContext.DefType(name);

    if (!m_IsTemplate && !ref)
        ref = m_TypeContext.GetIncompleteType(name);

    TypePtr type;
    if ((m_IsTemplate && (Expect("="), true)) || NextAt("="))
        type = ParseType();

    if (m_IsTemplate)
    {
        m_TypeContext.PopTemplate();
        m_IsTemplate = false;
        m_TemplateContext.InsertType(m_TemplateWhere, name, template_arguments, m_TemplateBuffer);
        return;
    }

    if (type && ref->IsIncomplete())
        ref = type;
}
