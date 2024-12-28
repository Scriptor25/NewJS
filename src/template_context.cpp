#include <sstream>
#include <NJS/AST.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TemplateContext.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>

NJS::TemplateContext::TemplateContext(Builder& builder)
    : m_Builder(builder)
{
}

void NJS::TemplateContext::InsertType(
    const std::string& name,
    const std::vector<std::string>& args,
    const SourceLocation& where,
    const std::string& source)
{
    m_TypeTemplates[name] = {name, args, where, source};
}

void NJS::TemplateContext::InsertFunction(
    const std::string& name,
    const std::vector<std::string>& args,
    const SourceLocation& where,
    const std::string& source)
{
    m_FunctionTemplates[name] = {name, args, where, source};
}

bool NJS::TemplateContext::HasFunction(const std::string& name) const
{
    return m_FunctionTemplates.contains(name);
}

bool NJS::TemplateContext::HasType(const std::string& name) const
{
    return m_TypeTemplates.contains(name);
}

std::string NJS::TemplateContext::InflateFunctionTemplate(
    Parser& parent,
    const std::string& templ_name,
    const std::vector<TypePtr>& args)
{
    const auto& [name_, args_, where_, source_] = m_FunctionTemplates[templ_name];

    auto name = name_ + '<';
    for (unsigned i = 0; i < args.size(); ++i)
    {
        if (i > 0) name += ", ";
        name += args[i]->GetString();
    }
    name += '>';

    auto& ref = m_InflatedFunctions[name];
    if (ref) return name;

    parent.m_TypeCtx.PushTemplate(args_, args);

    std::stringstream stream('?' + source_, std::ios_base::in);
    Parser parser(parent.m_TypeCtx, parent.m_TemplateCtx, stream, where_, parent.m_Macros);
    const auto inflated = std::dynamic_pointer_cast<FunctionExpr>(parser.ParseFunctionExpr());
    ref = std::make_shared<FunctionStmt>(
        inflated->Where,
        true,
        FnType_Function,
        name,
        inflated->Args,
        inflated->VarArg,
        inflated->ResultType,
        inflated->Body);
    ref->GenVoidLLVM(m_Builder);

    parent.m_TypeCtx.PopTemplate();

    return name;
}

NJS::TypePtr NJS::TemplateContext::InflateType(
    Parser& parent,
    const std::string& templ_name,
    const std::vector<TypePtr>& args)
{
    const auto& [name_, args_, where_, source_] = m_TypeTemplates[templ_name];

    auto name = name_ + '<';
    for (unsigned i = 0; i < args.size(); ++i)
    {
        if (i > 0) name += ", ";
        name += args[i]->GetString();
    }
    name += '>';

    auto& ref = m_InflatedTypes[name];
    if (ref) return ref;

    parent.m_TypeCtx.PushTemplate(args_, args);

    std::stringstream stream(source_, std::ios_base::in);
    Parser parser(parent.m_TypeCtx, parent.m_TemplateCtx, stream, where_, parent.m_Macros);
    const auto inflated = parser.ParseType();

    parent.m_TypeCtx.PopTemplate();

    return ref = inflated;
}
