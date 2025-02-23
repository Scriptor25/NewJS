#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::Builder::CreateSubscript(const SourceLocation &where, ValuePtr array, ValuePtr index)
{
    if (auto [
            result_,
            left_,
            right_,
            callee_
        ] = FindOperator("[", array, index);
        callee_)
    {
        const auto function_type = llvm::FunctionType::get(
            result_.GetLLVM(where, *this),
            {
                left_.GetLLVM(where, *this),
                right_.GetLLVM(where, *this),
            },
            false);
        if (left_.IsReference && !array->IsLValue())
        {
            const auto value = CreateAlloca(where, array->GetType(), true);
            value->StoreForce(where, array);
            array = value;
        }
        if (right_.IsReference && !index->IsLValue())
        {
            const auto value = CreateAlloca(where, index->GetType(), true);
            value->StoreForce(where, index);
            index = value;
        }
        const auto result_value = GetBuilder().CreateCall(
            function_type,
            callee_,
            {
                left_.IsReference
                    ? array->GetPtr(where)
                    : array->Load(where),
                right_.IsReference
                    ? index->GetPtr(where)
                    : index->Load(where),
            });
        if (result_.IsReference)
            return LValue::Create(*this, result_.Type, result_value, result_.IsConst);
        return RValue::Create(*this, result_.Type, result_value);
    }

    const auto array_type = array->GetType();
    const auto index_value = index->Load(where);

    if (array_type->IsPointer())
    {
        const auto element_type = array_type->GetElement(where);
        const auto ptr = GetBuilder().CreateGEP(
            element_type->GetLLVM(where, *this),
            array->Load(where),
            {index_value});
        return LValue::Create(*this, element_type, ptr, array_type->IsConst(where));
    }

    if (!array_type->IsArray() && !array_type->IsTuple())
        Error(where, "no subscript into type {} with index type {}", array_type, index->GetType());

    const auto const_index = llvm::dyn_cast<llvm::ConstantInt>(index_value);
    if (!const_index && !array->IsLValue())
    {
        const auto value = CreateAlloca(where, array_type, array->IsConst());
        value->StoreForce(where, array);
        array = value;
    }

    if (array->IsLValue())
    {
        const auto index_type = index_value->getType();
        const auto zero = llvm::Constant::getNullValue(index_type);
        const auto ptr = GetBuilder().CreateGEP(
            array_type->GetLLVM(where, *this),
            array->GetPtr(where),
            {zero, index_value});

        TypePtr type;
        if (array_type->IsArray())
            type = array_type->GetElement(where);
        else if (array_type->IsTuple())
        {
            if (!const_index)
                Error(where, "subscript index for indexing into tuple must be a constant");
            const auto i = const_index->getValue().getLimitedValue();
            type = array_type->GetElement(where, i);
        }
        return LValue::Create(*this, type, ptr, array->IsConst());
    }

    if (!const_index)
        Error(where, "subscript index for indexing into constant must be a constant");

    const auto i = const_index->getValue().getLimitedValue();
    const auto val = GetBuilder().CreateExtractValue(array->Load(where), i);
    return RValue::Create(*this, array_type->GetElement(where, i), val);
}

NJS::ValuePtr NJS::Builder::CreateSubscript(const SourceLocation &where, const ValuePtr &array, const unsigned index)
{
    return CreateSubscript(
        where,
        array,
        RValue::Create(
            *this,
            GetTypeContext().GetIntegerType(64, false),
            GetBuilder().getInt64(index)));
}
