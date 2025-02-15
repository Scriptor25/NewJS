#include <sstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TemplateContext::TemplateContext(Builder &builder)
    : m_Builder(builder)
{
}

void NJS::TemplateContext::InsertType(
    const std::string &name,
    const std::vector<std::string> &args,
    const SourceLocation &where,
    const std::string &source)
{
    m_TypeTemplates[std::string(name)] = {std::string(name), args, where, std::string(source)};
}

void NJS::TemplateContext::InsertFunction(
    const std::string &name,
    const std::vector<std::string> &args,
    const SourceLocation &where,
    const std::string &source)
{
    m_FunctionTemplates[std::string(name)] = {std::string(name), args, where, std::string(source)};
}

bool NJS::TemplateContext::HasFunction(const std::string &name) const
{
    return m_FunctionTemplates.contains(std::string(name));
}

bool NJS::TemplateContext::HasType(const std::string &name) const
{
    return m_TypeTemplates.contains(std::string(name));
}

std::string NJS::TemplateContext::InflateFunctionTemplate(
    Parser &parent,
    const std::string &templ_name,
    const std::vector<TypePtr> &args)
{
    const auto &[name_, args_, where_, source_] = m_FunctionTemplates[templ_name];

    auto name = name_ + '<';
    for (unsigned i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            name += ", ";
        name += args[i]->GetString();
    }
    name += '>';

    if (m_InflatedFunctions.contains(name))
        return name;
    m_InflatedFunctions.emplace(name);

    parent.m_TypeContext.PushTemplate(args_, args);

    std::stringstream stream('?' + source_, std::ios_base::in);
    Parser parser(parent.m_TypeContext, parent.m_TemplateContext, stream, where_, parent.m_MacroMap);
    const auto inflated = std::dynamic_pointer_cast<FunctionExpression>(parser.ParseFunctionExpression());
    FunctionStatement(
        inflated->Where,
        FunctionFlags_Absolute,
        name,
        inflated->Parameters,
        inflated->IsVarArg,
        inflated->ResultType,
        inflated->Body).GenVoidLLVM(m_Builder);

    parent.m_TypeContext.PopTemplate();
    return name;
}

NJS::TypePtr NJS::TemplateContext::InflateType(
    Parser &parent,
    const std::string &templ_name,
    const std::vector<TypePtr> &args)
{
    const auto &[name_, args_, where_, source_] = m_TypeTemplates[templ_name];

    auto name = name_ + '<';
    for (unsigned i = 0; i < args.size(); ++i)
    {
        if (i > 0)
            name += ", ";
        name += args[i]->GetString();
    }
    name += '>';

    auto &ref = m_InflatedTypes[name];
    if (ref)
        return ref;

    parent.m_TypeContext.PushTemplate(args_, args);

    std::stringstream stream(source_, std::ios_base::in);
    Parser parser(parent.m_TypeContext, parent.m_TemplateContext, stream, where_, parent.m_MacroMap);
    const auto inflated = parser.ParseType();

    parent.m_TypeContext.PopTemplate();

    return ref = inflated;
}
