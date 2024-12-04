#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
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

void NJS::DestructureArray::CreateVars(Builder& builder, const bool is_const, const ValuePtr& value)
{
    const auto type = Type ? Type : value->GetType();

    if (value->IsL())
    {
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto element_type = type->Element(i);
            const auto element = LValue::Create(
                builder,
                element_type,
                builder.LLVMBuilder().CreateStructGEP(type->GenLLVM(builder), value->GetPtr(), i));
            Elements[i]->CreateVars(builder, is_const, element);
        }
    }
    else
    {
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto element_type = type->Element(i);
            const auto element = RValue::Create(
                builder,
                element_type,
                builder.LLVMBuilder().CreateExtractValue(value->Load(), i));
            Elements[i]->CreateVars(builder, is_const, element);
        }
    }
}

std::ostream& NJS::DestructureArray::Print(std::ostream& os)
{
    os << "[ ";
    for (size_t i = 0; i < Elements.size(); ++i)
    {
        if (i > 0) os << ", ";
        Elements[i]->Print(os);
    }
    os << " ]";
    if (Type) Type->Print(os << ": ");
    return os;
}
