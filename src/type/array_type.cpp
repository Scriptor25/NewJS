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

bool NJS::ArrayType::IsComplex()
{
    return true;
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
    return TypeId_Array;
}

llvm::Type* NJS::ArrayType::GenLLVM(Builder& builder) const
{
    std::vector<llvm::Type*> elements(2);
    elements[0] = builder.LLVMBuilder().getPtrTy();
    elements[1] = builder.LLVMBuilder().getInt64Ty();
    return llvm::StructType::get(builder.LLVMContext(), elements);
}
