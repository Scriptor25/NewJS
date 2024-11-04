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

void NJS::DestructureArray::CreateVars(Context& ctx, const TypePtr& type)
{
    if (type && Type && type != Type) Error("cannot assign value of type {} to value of type {}", type, Type);
    for (size_t i = 0; i < Elements.size(); ++i)
        Elements[i]->CreateVars(ctx, (Type ? Type : type)->Element(i));
}

void NJS::DestructureArray::CreateVars(Builder& builder, bool is_const, ValuePtr value)
{
    if (!value->IsL())
    {
        value = builder.CreateAlloca(value->GetType());
        value->Store(value->Load());
    }

    const auto llvm_type = Type->GenLLVM(builder);
    const auto ptr = value->GetPtr();

    for (size_t i = 0; i < Elements.size(); ++i)
    {
        const auto element_type = Type->Element(i);
        const auto llvm_element_type = element_type->GenLLVM(builder);

        llvm::Value* gep;
        if (Type->IsTuple()) gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, ptr, i);
        else gep = builder.LLVMBuilder().CreateConstGEP1_64(llvm_element_type, ptr, i);

        Elements[i]->CreateVars(builder, is_const, LValue::Create(builder, element_type, gep));
    }
}

std::ostream& NJS::DestructureArray::Print(std::ostream& os)
{
    os << "[ ";
    for (size_t i = 0; i < Elements.size(); ++i)
    {
        if (i > 0) os << ", ";
        os << Elements[i];
    }
    os << " ]";
    if (Type) os << ": " << Type;
    return os;
}
