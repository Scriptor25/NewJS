#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::DestructureStruct::DestructureStruct(
    SourceLocation where,
    std::map<std::string, ParameterPtr> elements,
    ReferenceInfo info)
    : Parameter(std::move(where), {}, std::move(info)),
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
    const bool is_export,
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

    for (const auto &[name_, element_]: Elements)
    {
        const auto [value_, is_reference_] = builder.CreateMember(value, name_);
        element_->CreateVars(
            builder,
            value_,
            is_export,
            is_extern,
            is_reference_ ? value_->IsConst() : is_const,
            is_reference_ || is_reference);
    }
}

std::ostream &NJS::DestructureStruct::Print(std::ostream &stream, bool with_info)
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
        element->Print(stream << name << ": ", false);
    }
    stream << " }";
    if (Info.Type)
        Info.Type->Print(stream << ": ");
    return stream;
}
