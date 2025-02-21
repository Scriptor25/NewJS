#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::DestructureTuple::DestructureTuple(
    SourceLocation where,
    std::vector<ParameterPtr> elements,
    TypePtr type)
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
    const bool is_extern,
    const bool is_const,
    const bool is_reference)
{
    if (Type)
    {
        if (is_reference)
        {
            if (value->GetType() != Type)
                Error(
                    Where,
                    "type mismatch: cannot create reference with type {} from value of type {}",
                    Type,
                    value->GetType());
            if (value->IsConst() && !is_const)
                Error(Where, "cannot reference constant value as mutable");
        }
        else
            value = builder.CreateCast(Where, value, Type);
    }

    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto element = builder.CreateSubscript(Where, value, i);
        Elements[i]->CreateVars(builder, element, is_extern, is_const, is_reference);
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
