#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::DestructureTuple::DestructureTuple(
    SourceLocation where,
    std::vector<ParameterPtr> elements,
    ReferenceInfo info)
    : Parameter(std::move(where), {}, std::move(info)),
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
    if (is_reference)
    {
        if (Info.Type && value->GetType() != Info.Type)
            Error(Where, "cannot create reference of type {} to value of type {}", Info.Type, value->GetType());
        if (value->IsConst() && !is_const)
            Error(Where, "cannot create non-constant reference from constant value");
    }
    else if (Info.Type)
    {
        value = builder.CreateCast(value, Info.Type);
    }

    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto element = builder.CreateSubscript(value, i);
        Elements[i]->CreateVars(builder, element, is_extern, is_const, is_reference);
    }
}

std::ostream &NJS::DestructureTuple::Print(std::ostream &stream, bool with_info)
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
        Elements[i]->Print(stream, false);
    }
    stream << " ]";
    if (Info.Type)
        Info.Type->Print(stream << ": ");
    return stream;
}
