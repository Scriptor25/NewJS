#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::DestructureTuple::DestructureTuple(
    SourceLocation where,
    std::vector<ParameterPtr> elements,
    TypePtr type,
    ReferenceInfo info)
    : Parameter(std::move(where), {}, std::move(type), std::move(info)),
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
                return;
            if (value->IsConst() && !is_const)
                return;
        }
        else
        {
            value = builder.CreateCast(value, Type);
        }
    }

    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto element = builder.CreateSubscript(value, i);
        Elements[i]->CreateVars(builder, element, is_extern, is_const, is_reference);
    }
}

std::ostream &NJS::DestructureTuple::Print(std::ostream &stream)
{
    if (Info.IsReference)
    {
        if (Info.IsConst)
            stream << "const ";
        stream << "&";
    }
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
