#include <utility>
#include <llvm/IR/Value.h>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::CallExpression::CallExpression(SourceLocation where, ExpressionPtr callee, std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      Callee(std::move(callee)),
      Arguments(std::move(arguments))
{
}

NJS::ValuePtr NJS::CallExpression::GenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    const auto callee = Callee->GenLLVM(builder, {});
    const auto callee_type = std::dynamic_pointer_cast<FunctionType>(callee->GetType());
    if (!callee_type)
        Error(Where, "invalid callee: callee is not a function");

    const auto parameter_count = callee_type->GetParameterCount(Callee->Where);
    if (Arguments.size() < parameter_count)
        Error(Where, "not enough arguments");
    if (Arguments.size() > parameter_count && !callee_type->IsVarArg(Callee->Where))
        Error(Where, "too many arguments");

    std::vector<llvm::Value *> arguments(Arguments.size());
    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        auto [
            type_,
            is_const_,
            is_reference_
        ] = (i < parameter_count)
                ? callee_type->GetParameter(Callee->Where, i)
                : ReferenceInfo();

        auto &argument = Arguments[i];
        auto argument_value = argument->GenLLVM(builder, type_);

        if (!is_reference_)
        {
            if (type_)
                argument_value = builder.CreateCast(argument->Where, argument_value, type_);
            arguments[i] = argument_value->Load(argument->Where);
            continue;
        }

        if (argument_value->GetType() != type_)
            Error(
                Where,
                "type mismatch: cannot create reference with type {} from value of type {}",
                type_,
                argument_value->GetType());
        if (argument_value->IsConst() && !is_const_)
            Error(Where, "cannot reference constant value as mutable");

        if (!argument_value->IsLValue())
        {
            if (!is_const_)
                Error(Where, "cannot create mutable reference to constant");
            auto value = builder.CreateAlloca(Where, argument_value->GetType(), true);
            value->StoreForce(Where, argument_value);
            argument_value = value;
        }

        arguments[i] = argument_value->GetPtr(argument->Where);
    }

    const auto result_value = builder.GetBuilder().CreateCall(
        callee_type->GenFnLLVM(Callee->Where, builder),
        callee->Load(Callee->Where),
        arguments);

    auto [
        type_,
        is_const_,
        is_reference_
    ] = callee_type->GetResult(Callee->Where);
    if (is_reference_)
        return LValue::Create(builder, type_, result_value, is_const_);
    return RValue::Create(builder, type_, result_value);
}

std::ostream &NJS::CallExpression::Print(std::ostream &stream)
{
    Callee->Print(stream) << '(';
    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Arguments[i]->Print(stream);
    }
    return stream << ')';
}
