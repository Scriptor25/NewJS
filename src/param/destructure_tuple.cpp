#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::DestructureTuple::DestructureTuple(SourceLocation where, std::vector<ParameterPtr> elements, TypePtr type)
    : Parameter(std::move(where), {}, std::move(type)),
      Elements(std::move(elements))
{
}

bool NJS::DestructureTuple::RequireValue()
{
    return true;
}

void NJS::DestructureTuple::CreateVars(
    Builder &builder,
    ValuePtr value,
    const unsigned flags)
{
    if (Type)
    {
        if (Type->IsReference())
        {
            if (value->GetType() != Type->GetElement(Where))
                Error(
                    Where,
                    "type mismatch: cannot create reference with type {} from value of type {}",
                    Type->GetElement(Where),
                    value->GetType());
        }
        else
            value = builder.CreateCast(Where, value, Type);
    }

    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto element = builder.CreateSubscript(Where, value, i);
        Elements[i]->CreateVars(builder, element, flags);
    }
}

std::ostream &NJS::DestructureTuple::Print(std::ostream &stream)
{
    stream << "[ ";
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Elements[i]->Print(stream);
    }
    stream << " ]";
    if (Type)
        Type->Print(stream << ": ");
    return stream;
}
