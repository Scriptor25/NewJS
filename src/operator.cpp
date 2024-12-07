#include <NJS/Builder.hpp>
#include <NJS/Operator.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::ValuePtr NJS::OperatorEQ(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* value;

    if (lhs->GetType()->IsPrimitive(Primitive_Number) && rhs->GetType()->IsPrimitive(Primitive_Number))
        value = builder.GetBuilder().CreateFCmpOEQ(lhs->Load(), rhs->Load());
    else if ((lhs->GetType()->IsPrimitive(Primitive_Boolean) && rhs->GetType()->IsPrimitive(Primitive_Boolean))
        || (lhs->GetType()->IsPrimitive(Primitive_Char) && rhs->GetType()->IsPrimitive(Primitive_Char)))
        value = builder.GetBuilder().CreateICmpEQ(lhs->Load(), rhs->Load());
    else return {};

    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorNE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    llvm::Value* value;

    if (lhs->GetType()->IsPrimitive(Primitive_Number) && rhs->GetType()->IsPrimitive(Primitive_Number))
        value = builder.GetBuilder().CreateFCmpONE(lhs->Load(), rhs->Load());
    else if ((lhs->GetType()->IsPrimitive(Primitive_Boolean) && rhs->GetType()->IsPrimitive(Primitive_Boolean))
        || (lhs->GetType()->IsPrimitive(Primitive_Char) && rhs->GetType()->IsPrimitive(Primitive_Char)))
        value = builder.GetBuilder().CreateICmpNE(lhs->Load(), rhs->Load());
    else return {};

    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOLT(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOLE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorGT(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOGT(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorGE(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFCmpOGE(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Boolean) || !rhs->GetType()->IsPrimitive(Primitive_Boolean))
        return {};

    const auto value = builder.GetBuilder().CreateOr(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Boolean) || !rhs->GetType()->IsPrimitive(Primitive_Boolean))
        return {};

    const auto value = builder.GetBuilder().CreateXor(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorLAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Boolean) || !rhs->GetType()->IsPrimitive(Primitive_Boolean))
        return {};

    const auto value = builder.GetBuilder().CreateAnd(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetBooleanType(), value);
}

NJS::ValuePtr NJS::OperatorOr(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateOr(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorXor(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateXor(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorAnd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateAnd(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorAdd(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFAdd(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorSub(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFSub(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorMul(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFMul(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorDiv(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFDiv(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorRem(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateFRem(lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorPow(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto value = builder.GetBuilder().CreateBinaryIntrinsic(llvm::Intrinsic::pow, lhs->Load(), rhs->Load());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorShL(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateShl(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::ValuePtr NJS::OperatorShR(Builder& builder, const ValuePtr& lhs, const ValuePtr& rhs)
{
    if (!lhs->GetType()->IsPrimitive(Primitive_Number) || !rhs->GetType()->IsPrimitive(Primitive_Number))
        return {};

    const auto l = builder.GetBuilder().CreateFPToSI(lhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto r = builder.GetBuilder().CreateFPToSI(rhs->Load(), builder.GetBuilder().getInt64Ty());
    const auto result = builder.GetBuilder().CreateAShr(l, r);
    const auto value = builder.GetBuilder().CreateSIToFP(result, builder.GetBuilder().getDoubleTy());
    return RValue::Create(builder, builder.GetCtx().GetNumberType(), value);
}

NJS::TypePtr NJS::OperatorType(TypeContext& ctx, std::string op, const TypePtr& lhs, const TypePtr& rhs)
{
    static const auto fn_cmp_eq = [&ctx](const TypePtr&, const TypePtr&) -> TypePtr
    {
        return ctx.GetBooleanType();
    };

    static const auto fn_cmp = [&ctx](const TypePtr& l, const TypePtr& r) -> TypePtr
    {
        if (!l->IsPrimitive(Primitive_Number) || !r->IsPrimitive(Primitive_Number))
            return {};
        return ctx.GetBooleanType();
    };

    static const auto fn_arith = [&ctx](const TypePtr& l, const TypePtr& r) -> TypePtr
    {
        if (!l->IsPrimitive(Primitive_Number) || !r->IsPrimitive(Primitive_Number))
            return {};
        return ctx.GetNumberType();
    };

    static const std::map<std::string, std::function<TypePtr(TypePtr, TypePtr)>> ops
    {
        {"==", fn_cmp_eq},
        {"!=", fn_cmp_eq},
        {"<", fn_cmp},
        {"<=", fn_cmp},
        {">", fn_cmp},
        {">=", fn_cmp},
        {"+", fn_arith},
        {"-", fn_arith},
        {"*", fn_arith},
        {"/", fn_arith},
        {"%", fn_arith},
        {"**", fn_arith},
        {"|", fn_arith},
        {"^", fn_arith},
        {"&", fn_arith},
        {"<<", fn_arith},
        {">>", fn_arith},
    };

    if (ops.contains(op))
        return ops.at(op)(lhs, rhs);

    if (op.back() == '=')
    {
        op.pop_back();
        if (ops.contains(op))
            return ops.at(op)(lhs, rhs);
    }

    return {};
}
