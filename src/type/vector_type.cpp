#include <NJS/Builder.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::VectorType::GenString(const TypePtr& element_type)
{
    return element_type->String + "[]";
}

NJS::VectorType::VectorType(TypePtr element_type)
    : Type(GenString(element_type)), ElementType(std::move(element_type))
{
}

bool NJS::VectorType::IsVector() const
{
    return true;
}

NJS::TypePtr NJS::VectorType::Element()
{
    return ElementType;
}

NJS::TypePtr NJS::VectorType::Element(size_t)
{
    return ElementType;
}

void NJS::VectorType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    args.push_back(builder.GetBuilder().getInt32(ID_VECTOR));
    ElementType->TypeInfo(builder, args);
}

size_t NJS::VectorType::Bytes() const
{
    return 8;
}

llvm::Type* NJS::VectorType::GenLLVM(Builder& builder) const
{
    return builder.GetBuilder().getPtrTy();
}

llvm::StructType* NJS::VectorType::GenVecLLVM(const Builder& builder)
{
    const auto ptr_ty = builder.GetBuilder().getPtrTy();
    const auto size_ty = builder.GetBuilder().getInt64Ty();
    return llvm::StructType::get(ptr_ty, size_ty);
}
