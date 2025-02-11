#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateSubscript(const SourceLocation &where, const ValuePtr &array, const ValuePtr &index)
{
    return CreateSubscript(where, array, index->Load(where));
}

NJS::ValuePtr NJS::Builder::CreateSubscript(const SourceLocation &where, const ValuePtr &array, const unsigned index)
{
    return CreateSubscript(
        where,
        array,
        RValue::Create(
            *this,
            GetCtx().GetIntType(64, false),
            GetBuilder().getInt64(index)));
}

NJS::ValuePtr NJS::Builder::CreateSubscript(const SourceLocation &where, const ValuePtr &array, llvm::Value *index)
{
    const auto array_type = array->GetType();

    if (array_type->IsPtr())
    {
        const auto element_type = array_type->GetElement();
        const auto ptr = GetBuilder().CreateGEP(
            element_type->GetLLVM(where, *this),
            array->Load(where),
            {index});
        return LValue::Create(*this, element_type, ptr);
    }

    if (array->IsL())
    {
        const auto index_type = index->getType();
        const auto zero = llvm::Constant::getNullValue(index_type);
        const auto ptr = GetBuilder().CreateGEP(
            array_type->GetLLVM(where, *this),
            array->GetPtr(where),
            {zero, index});

        TypePtr type;
        if (array_type->IsArray())
        {
            type = array_type->GetElement();
        }
        else if (array_type->IsTuple())
        {
            const auto i = llvm::dyn_cast<llvm::ConstantInt>(index)->getValue().getLimitedValue();
            type = array_type->GetElement(i);
        }
        return LValue::Create(*this, type, ptr);
    }

    const auto i = llvm::dyn_cast<llvm::ConstantInt>(index)->getValue().getLimitedValue();
    const auto val = GetBuilder().CreateExtractValue(array->Load(where), i);
    return RValue::Create(*this, array_type->GetElement(i), val);
}
