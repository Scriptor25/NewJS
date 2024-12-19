#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::DestructureArray::DestructureArray(std::vector<ParamPtr> elements)
    : Param(""), Elements(std::move(elements))
{
}

bool NJS::DestructureArray::RequireValue()
{
    return true;
}

void NJS::DestructureArray::CreateVars(Parser& parser, const TypePtr& val_type)
{
    const auto type = Type
                          ? Type->IsRef()
                                ? Type->GetElement()
                                : Type
                          : val_type;

    for (unsigned i = 0; i < Elements.size(); ++i)
        Elements[i]->CreateVars(parser, type->GetElement(i));
}

void NJS::DestructureArray::CreateVars(Builder& builder, const bool is_const, const ValuePtr& value)
{
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        const auto element = builder.CreateSubscript(value, i);
        Elements[i]->CreateVars(builder, is_const, element);
    }
}

std::ostream& NJS::DestructureArray::Print(std::ostream& os)
{
    os << "[ ";
    for (unsigned i = 0; i < Elements.size(); ++i)
    {
        if (i > 0) os << ", ";
        Elements[i]->Print(os);
    }
    os << " ]";
    if (Type) Type->Print(os << ": ");
    return os;
}
