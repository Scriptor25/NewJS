#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::DestructureStruct::DestructureStruct(
    SourceLocation where,
    std::map<std::string, ParameterPtr> elements,
    TypePtr type)
    : Parameter(std::move(where), {}, std::move(type)),
      Elements(std::move(elements))
{
}

bool NJS::DestructureStruct::RequireValue()
{
    return true;
}

void NJS::DestructureStruct::CreateVars(
    Builder &builder,
    ValuePtr value,
    const unsigned flags)
{
    if (Type)
    {
        if (Type->IsReference())
        {
            if (value->GetType() != Type)
                Error(
                    Where,
                    "type mismatch: cannot create reference with type {} from value of type {}",
                    Type->GetElement(Where),
                    value->GetType());
        }
        else
            value = builder.CreateCast(Where, value, Type);
    }

    for (const auto &[name_, element_]: Elements)
    {
        const auto member = builder.CreateMember(Where, value, name_);
        element_->CreateVars(builder, member, flags);
    }
}

std::ostream &NJS::DestructureStruct::Print(std::ostream &stream)
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
