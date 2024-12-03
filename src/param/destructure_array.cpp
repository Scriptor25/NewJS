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
        const auto ptr = value->GetPtr();
        const auto ty = type->GenLLVM(builder);
        const auto data_ptr = builder.LLVMBuilder().CreateStructGEP(ty, ptr, 1);
        const auto data_ty = type->GenBaseLLVM(builder);

        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto element_type = type->Element(i);
            const auto llvm_element_type = element_type->GenLLVM(builder);

            llvm::Value* gep;
            if (type->IsTuple()) gep = builder.LLVMBuilder().CreateStructGEP(data_ty, data_ptr, i);
            else
            {
                const auto ptr_to_base_ptr = builder.LLVMBuilder().CreateStructGEP(data_ty, data_ptr, 0);
                const auto base_ptr = builder.LLVMBuilder().CreateLoad(
                    builder.LLVMBuilder().getPtrTy(),
                    ptr_to_base_ptr);
                gep = builder.LLVMBuilder().CreateConstGEP1_64(llvm_element_type, base_ptr, i);
            }

            Elements[i]->CreateVars(builder, is_const, LValue::Create(builder, element_type, gep));
        }
    }
    else
    {
        for (size_t i = 0; i < Elements.size(); ++i)
        {
            const auto element_type = type->Element(i);
            const auto llvm_element_type = element_type->GenLLVM(builder);

            ValuePtr member;
            if (type->IsTuple())
                member = RValue::Create(
                    builder,
                    element_type,
                    builder.LLVMBuilder().CreateExtractValue(value->Load(), {1, i}));
            else
            {
                const auto base_ptr = builder.LLVMBuilder().CreateExtractValue(value->Load(), {1, 0});
                member = LValue::Create(
                    builder,
                    element_type,
                    builder.LLVMBuilder().CreateConstGEP1_64(llvm_element_type, base_ptr, i));
            }

            Elements[i]->CreateVars(builder, is_const, member);
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
