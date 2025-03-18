#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::ValuePtr NJS::Builder::CreateSubscript(ValuePtr array, const ValuePtr &index) const
{
    if (auto [
            result_,
            left_,
            right_,
            callee_
        ] = FindOperator("[]", array, index);
        callee_)
    {
        const auto function_type = llvm::FunctionType::get(
            result_.GetLLVM(*this),
            {left_.GetLLVM(*this), right_.GetLLVM(*this)},
            false);

        const auto array_value = left_.SolveFor(*this, array);
        const auto index_value = right_.SolveFor(*this, index);

        const auto result_value = GetBuilder().CreateCall(
            function_type,
            callee_,
            {array_value, index_value});
        if (result_.IsReference)
            return LValue::Create(*this, result_.Type, result_value, result_.IsConst);
        return RValue::Create(*this, result_.Type, result_value);
    }

    const auto index_value = index->Load();

    if (array->GetType()->IsPointer())
    {
        const auto element_type = Type::As<PointerType>(array->GetType())->GetElement();
        const auto pointer = GetBuilder().CreateGEP(
            element_type->GetLLVM(*this),
            array->Load(),
            {index_value});
        return LValue::Create(*this, element_type, pointer, Type::As<PointerType>(array->GetType())->IsConst());
    }

    if (!array->GetType()->IsArray() && !array->GetType()->IsTuple())
        Error(
            "cannot create subscript into value of type {} with index of type {}",
            array->GetType(),
            index->GetType());

    const auto const_index = llvm::dyn_cast<llvm::ConstantInt>(index_value);
    if (!const_index && !array->IsLValue())
    {
        const auto value = CreateAlloca(array->GetType(), array->IsConst());
        value->StoreNoError(array);
        array = value;
    }

    if (array->IsLValue())
    {
        const auto zero = llvm::Constant::getNullValue(index_value->getType());
        const auto pointer = GetBuilder().CreateGEP(
            array->GetType()->GetLLVM(*this),
            array->GetPointer(),
            {zero, index_value});

        TypePtr type;
        if (array->GetType()->IsArray())
        {
            type = Type::As<ArrayType>(array->GetType())->GetElement();
        }
        else if (array->GetType()->IsTuple())
        {
            if (!const_index)
                Error("cannot create subscript into tuple with non-constant index");

            const auto i = const_index->getValue().getLimitedValue();
            type = Type::As<TupleType>(array->GetType())->GetElement(i);
        }
        return LValue::Create(*this, type, pointer, array->IsConst());
    }

    if (!const_index)
        Error("cannot create subscript into constant array or tuple with non-constant index");

    const auto i = const_index->getValue().getLimitedValue();
    const auto element_value = GetBuilder().CreateExtractValue(array->Load(), i);
    const auto element_type = array->GetType()->IsArray()
                                  ? Type::As<ArrayType>(array->GetType())->GetElement()
                                  : Type::As<TupleType>(array->GetType())->GetElement(i);

    return RValue::Create(*this, element_type, element_value);
}

NJS::ValuePtr NJS::Builder::CreateSubscript(const ValuePtr &array, const unsigned index) const
{
    return CreateSubscript(
        array,
        RValue::Create(
            *this,
            GetTypeContext().GetIntegerType(64, false),
            GetBuilder().getInt64(index)));
}
