#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>

NJS::DestructureObject::DestructureObject(SourceLocation where, std::map<std::string, ParameterPtr> elements)
    : Parameter(std::move(where), {}),
      Elements(std::move(elements))
{
}

bool NJS::DestructureObject::RequireValue()
{
    return true;
}

void NJS::DestructureObject::CreateVars(
    Builder &builder,
    const ValuePtr value,
    const unsigned flags)
{
    for (const auto &[name_, element_]: Elements)
    {
        const auto member = builder.CreateMember(Where, value, name_);
        element_->CreateVars(builder, member, flags);
    }
}

std::ostream &NJS::DestructureObject::Print(std::ostream &stream)
{
    stream << "{ ";
    auto first = true;
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
