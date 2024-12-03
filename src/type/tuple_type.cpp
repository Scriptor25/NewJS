#include <llvm/IR/DerivedTypes.h>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>

std::string NJS::TupleType::GenString(const std::vector<TypePtr>& element_types)
{
    std::string str;

    str += "[ ";
    bool first = true;
    for (const auto& type : element_types)
    {
        if (first) first = false;
        else str += ", ";
        str += type->String;
    }
    return str + " ]";
}

NJS::TupleType::TupleType(std::vector<TypePtr> element_types)
    : Type(GenString(element_types)), ElementTypes(std::move(element_types))
{
}

bool NJS::TupleType::IsTuple()
{
    return true;
}

size_t NJS::TupleType::Size()
{
    size_t size = 0;
    for (const auto& type : ElementTypes)
        size += type->Size();
    return size;
}

NJS::TypePtr NJS::TupleType::Element(const size_t i)
{
    return ElementTypes[i];
}

NJS::TypeId NJS::TupleType::GetId() const
{
    return TypeId_Complex;
}

llvm::Type* NJS::TupleType::GenLLVM(Builder& builder) const
{
    const auto ptr_ty = builder.LLVMBuilder().getPtrTy();
    const auto tup_ty = GenBaseLLVM(builder);
    return llvm::StructType::get(ptr_ty, tup_ty);
}

llvm::Type* NJS::TupleType::GenBaseLLVM(Builder& builder) const
{
    std::vector<llvm::Type*> elements(ElementTypes.size());
    for (size_t i = 0; i < ElementTypes.size(); ++i)
        elements[i] = ElementTypes[i]->GenLLVM(builder);
    return llvm::StructType::get(builder.LLVMContext(), elements);
}
