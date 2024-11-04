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

llvm::Type* NJS::ArrayType::GenLLVM(Builder& builder)
{
    return builder.LLVMBuilder().getPtrTy();
}
