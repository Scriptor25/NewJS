#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::DestructureObject::DestructureObject(std::map<std::string, ParamPtr> elements)
    : Param(""), Elements(std::move(elements))
{
}

bool NJS::DestructureObject::RequireValue()
{
    return true;
}

void NJS::DestructureObject::CreateVars(Builder& builder, const bool is_const, const ValuePtr& value)
{
    const auto type = Type ? Type : value->GetType();

    if (value->IsL())
    {
        const auto ptr = value->GetPtr();
        const auto ty = type->GenLLVM(builder);
        const auto data_ptr = builder.LLVMBuilder().CreateStructGEP(ty, ptr, 1);
        const auto data_ty = type->GenBaseLLVM(builder);

        for (const auto& [name, element] : Elements)
        {
            const auto gep = builder.LLVMBuilder().CreateStructGEP(data_ty, data_ptr, type->MemberIndex(name));
            element->CreateVars(builder, is_const, LValue::Create(builder, type->Member(name), gep));
        }
    }
    else
    {
        for (const auto& [name, element] : Elements)
        {
            const auto member = builder.LLVMBuilder().CreateExtractValue(value->Load(), {1, type->MemberIndex(name)});
            element->CreateVars(builder, is_const, RValue::Create(builder, type->Member(name), member));
        }
    }
}

std::ostream& NJS::DestructureObject::Print(std::ostream& os)
{
    os << "{ ";
    bool first = true;
    for (const auto& [name, element] : Elements)
    {
        if (first) first = false;
        else os << ", ";
        element->Print(os << name << ": ");
    }
    os << " }";
    if (Type) Type->Print(os << ": ");
    return os;
}
