#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>

NJS::DestructureObject::DestructureObject(std::map<std::string, ParameterPtr> elements)
    : Parameter(""),
      Elements(std::move(elements))
{
}

bool NJS::DestructureObject::RequireValue()
{
    return true;
}

void NJS::DestructureObject::CreateVars(
    Builder &builder,
    const SourceLocation &where,
    const bool is_const,
    const ValuePtr &value)
{
    for (const auto &[name_, element_]: Elements)
    {
        const auto member = builder.CreateMember(where, value, name_);
        element_->CreateVars(builder, where, is_const, member);
    }
}

std::ostream &NJS::DestructureObject::Print(std::ostream &stream)
{
    stream << "{ ";
    bool first = true;
    for (const auto &[name, element]: Elements)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        element->Print(stream << name << ": ");
    }
    stream << " }";
    if (Type)
        Type->Print(stream << ": ");
    return stream;
}
