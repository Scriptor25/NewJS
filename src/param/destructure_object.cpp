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

void NJS::DestructureObject::CreateVars(Builder& builder, const bool is_const, ValuePtr value)
{
    if (!value->IsL())
    {
        value = builder.CreateAlloca(value->GetType());
        value->Store(value->Load());
    }

    const auto llvm_type = Type->GenLLVM(builder);
    const auto ptr = value->GetPtr();

    for (const auto& [name, element] : Elements)
    {
        const auto gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, ptr, Type->MemberIndex(name));
        element->CreateVars(builder, is_const, LValue::Create(builder, Type->Member(name), gep));
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
