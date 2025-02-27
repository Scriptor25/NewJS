#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::DestructureStruct::DestructureStruct(
    SourceLocation where,
    std::map<std::string, ParameterPtr> elements,
    TypePtr type,
    ReferenceInfo info)
    : Parameter(std::move(where), {}, std::move(type), std::move(info)),
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
    const bool is_extern,
    const bool is_const,
    const bool is_reference,
    ErrorInfo &error)
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

    for (const auto &[name_, element_]: Elements)
    {
        const auto member = builder.CreateMember(Where, value, name_);
        element_->CreateVars(builder, member, is_extern, is_const, is_reference, error);
    }
}

std::ostream &NJS::DestructureStruct::Print(std::ostream &stream)
{
    if (Info.IsReference)
    {
        if (Info.IsConst)
            stream << "const ";
        stream << "&";
    }
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
