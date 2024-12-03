#include <llvm/IR/DerivedTypes.h>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

std::string NJS::ArrayType::GenString(const TypePtr& element_type)
{
    return element_type->String + "[]";
}

NJS::ArrayType::ArrayType(TypePtr element_type)
    : Type(GenString(element_type)), ElementType(std::move(element_type))
{
}

NJS::TypePtr NJS::ArrayType::Element()
{
    return ElementType;
}

NJS::TypePtr NJS::ArrayType::Element(const size_t i)
{
    return ElementType;
}

size_t NJS::ArrayType::ElementSize()
{
    return ElementType->Size();
}

NJS::TypeId NJS::ArrayType::GetId() const
{
    return TypeId_Complex;
}

llvm::Type* NJS::ArrayType::GenLLVM(Builder& builder) const
{
    const auto ptr_ty = builder.LLVMBuilder().getPtrTy();
    const auto int_ty = builder.LLVMBuilder().getInt64Ty();
    const auto arr_ty = llvm::StructType::get(ptr_ty, int_ty);
    return llvm::StructType::get(ptr_ty, arr_ty);
}

llvm::Type* NJS::ArrayType::GenBaseLLVM(Builder& builder) const
{
    const auto ptr_ty = builder.LLVMBuilder().getPtrTy();
    const auto int_ty = builder.LLVMBuilder().getInt64Ty();
    return llvm::StructType::get(ptr_ty, int_ty);
}
