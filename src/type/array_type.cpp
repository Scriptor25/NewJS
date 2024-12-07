#include <llvm/IR/DerivedTypes.h>
#include <NJS/Builder.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::ArrayType::GenString(const TypePtr& element_type, const size_t element_count)
{
    return element_type->String + '[' + std::to_string(element_count) + ']';
}

NJS::ArrayType::ArrayType(TypeContext& ctx, TypePtr element_type, const size_t element_count)
    : Type(ctx, GenString(element_type, element_count)),
      ElementType(std::move(element_type)),
      ElementCount(element_count)
{
}

bool NJS::ArrayType::IsArray() const
{
    return true;
}

NJS::TypePtr NJS::ArrayType::Element() const
{
    return ElementType;
}

NJS::TypePtr NJS::ArrayType::Element(const size_t i) const
{
    return ElementType;
}

size_t NJS::ArrayType::NumElements() const
{
    return ElementCount;
}

void NJS::ArrayType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_ARRAY));
    args.push_back(builder.GetBuilder().getInt64(ElementCount));
    ElementType->TypeInfo(builder, args);
}

size_t NJS::ArrayType::Bytes() const
{
    return ElementCount * ElementType->Bytes();
}

llvm::Type* NJS::ArrayType::GenLLVM(Builder& builder) const
{
    return llvm::ArrayType::get(ElementType->GenLLVM(builder), ElementCount);
}
