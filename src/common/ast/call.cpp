#include <utility>
#include <llvm/IR/Value.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::CallExpression::CallExpression(SourceLocation where, ExpressionPtr callee, std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      Callee(std::move(callee)),
      Arguments(std::move(arguments))
{
}

NJS::ValuePtr NJS::CallExpression::PGenLLVM(Builder &builder, const TypePtr &expected_type) const
{
    const auto callee = Callee->GenLLVM(builder, {});

    if (!callee->GetType()->IsFunction())
        Error(Where, "cannot call non-function callee of type {}", callee->GetType());

    const auto function_type = Type::As<FunctionType>(callee->GetType());
    const auto parameter_count = function_type->GetParameterCount();

    llvm::Value *first_argument{};
    if (const auto member_expression = std::dynamic_pointer_cast<MemberExpression>(Callee);
        member_expression && Arguments.size() == parameter_count - 1)
    {
        auto object = member_expression->Object->GenLLVM(builder, {});
        if (member_expression->Dereference)
        {
            const auto pointer_type = Type::As<PointerType>(object->GetType());
            object = LValue::Create(builder, pointer_type->GetElement(), object->Load(), pointer_type->IsConst());
        }

        auto [
            type_,
            is_const_,
            is_reference_
        ] = function_type->GetParameter(0);

        if (type_ == object->GetType())
        {
            if (is_reference_)
            {
                if (!is_const_ && object->IsConst())
                    Error(
                        Where,
                        "cannot call optional-self style function requiring non-constant operand with constant caller");
                if (!object->IsLValue())
                {
                    const auto value = builder.CreateAlloca(object->GetType(), true);
                    value->StoreNoError(object);
                    object = value;
                }
            }
            first_argument = is_reference_ ? object->GetPointer() : object->Load();
        }
    }

    if (!first_argument)
    {
        if (Arguments.size() < parameter_count)
            Error(Where, "not enough arguments, {} < {}", Arguments.size(), parameter_count);
        if (Arguments.size() > parameter_count && !function_type->IsVarArg())
            Error(Where, "too many arguments, {} > {}", Arguments.size(), parameter_count);
    }

    const auto has_first = !!first_argument;

    std::vector<llvm::Value *> arguments(Arguments.size() + has_first);

    if (has_first)
        arguments[0] = first_argument;

    for (unsigned i = has_first; i < arguments.size(); ++i)
    {
        auto [
            type_,
            is_const_,
            is_reference_
        ] = i < parameter_count
                ? function_type->GetParameter(i)
                : ReferenceInfo();

        auto &argument = Arguments[i - has_first];
        auto argument_value = argument->GenLLVM(builder, type_);

        if (!is_reference_)
        {
            if (type_)
                argument_value = builder.CreateCast(argument_value, type_);
            arguments[i] = argument_value->Load();
            continue;
        }

        if (argument_value->GetType() != type_)
            Error(Where, "type mismatch, {} != {}", argument_value->GetType(), type_);

        if (argument_value->IsConst() && !is_const_)
            Error(Where, "cannot pass constant value as mutable");

        if (!argument_value->IsLValue())
        {
            const auto value = builder.CreateAlloca(argument_value->GetType(), true);
            value->StoreNoError(argument_value);
            argument_value = value;
        }

        arguments[i] = argument_value->GetPointer();
    }

    const auto result_value = builder.GetBuilder().CreateCall(
        function_type->GenFnLLVM(builder),
        callee->Load(),
        arguments);

    auto [
        type_,
        is_const_,
        is_reference_
    ] = function_type->GetResult();
    if (is_reference_)
        return LValue::Create(builder, type_, result_value, is_const_);
    return RValue::Create(builder, type_, result_value);
}

std::ostream &NJS::CallExpression::Print(std::ostream &stream) const
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
