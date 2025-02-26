#include <newjs/ast.hpp>
#include <newjs/parser.hpp>
#include <newjs/template_context.hpp>

NJS::TemplateContext::TemplateContext(Builder &builder)
    : m_Builder(builder)
{
}

NJS::Builder &NJS::TemplateContext::GetBuilder() const
{
    return m_Builder;
}

void NJS::TemplateContext::InsertType(
    const SourceLocation &where,
    const std::string &name,
    const std::vector<std::string> &parameters,
    const std::string &source)
{
    m_TypeTemplates[name] = {where, name, parameters, source};
}

void NJS::TemplateContext::InsertFunction(
    const SourceLocation &where,
    const std::string &name,
    const std::vector<std::string> &parameters,
    const std::string &source)
{
    m_FunctionTemplates[name] = {where, name, parameters, source};
}

bool NJS::TemplateContext::HasFunction(const std::string &name) const
{
    return m_FunctionTemplates.contains(name);
}

bool NJS::TemplateContext::HasType(const std::string &name) const
{
    return m_TypeTemplates.contains(name);
}

std::string NJS::TemplateContext::InflateFunction(
    Parser &parent,
    const std::string &template_name,
    const std::vector<TypePtr> &arguments)
{
    const auto &template_ = m_FunctionTemplates.at(template_name);
    auto name = template_.GetName(arguments);

    if (m_InflatedFunctions.contains(name))
        return name;
    m_InflatedFunctions.emplace(name);

    template_.InflateFunction(parent, arguments).GenVoidLLVM(m_Builder);
    return name;
}

NJS::TypePtr NJS::TemplateContext::InflateType(
    Parser &parent,
    const std::string &template_name,
    const std::vector<TypePtr> &arguments)
{
    const auto &template_ = m_TypeTemplates.at(template_name);
    const auto name = template_.GetName(arguments);

    auto &ref = m_InflatedTypes[name];
    if (!ref)
        ref = template_.InflateType(parent, arguments);
    return ref;
}
