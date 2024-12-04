#include <llvm/IR/DerivedTypes.h>
#include <NJS/Builder.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::ArrayType::GenString(const TypePtr& element_type, const size_t element_count)
{
    return element_type->String + '[' + std::to_string(element_count) + ']';
}

NJS::ArrayType::ArrayType(TypePtr element_type, const size_t element_count)
    : Type(GenString(element_type, element_count)), ElementType(std::move(element_type)), ElementCount(element_count)
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

void NJS::ArrayType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.LLVMBuilder().getInt32(ID_ARRAY));
    args.push_back(builder.LLVMBuilder().getInt64(ElementCount));
    ElementType->TypeInfo(builder, args);
}

llvm::Type* NJS::ArrayType::GenLLVM(Builder& builder) const
{
    return llvm::ArrayType::get(ElementType->GenLLVM(builder), ElementCount);
}
