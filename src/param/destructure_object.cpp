#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>

NJS::DestructureObject::DestructureObject(std::map<std::string, ParamPtr> elements)
    : Param(""),
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

std::ostream &NJS::DestructureObject::Print(std::ostream &os)
{
    os << "{ ";
    bool first = true;
    for (const auto &[name, element]: Elements)
    {
        if (first)
            first = false;
        else
            os << ", ";
        element->Print(os << name << ": ");
    }
    os << " }";
    if (Type)
        Type->Print(os << ": ");
    return os;
}
