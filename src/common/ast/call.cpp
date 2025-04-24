#include <utility>
#include <llvm/IR/Value.h>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

NJS::CallExpression::CallExpression(SourceLocation where, ExpressionPtr callee, std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      Callee(std::move(callee)),
      Arguments(std::move(arguments))
{
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

NJS::ValuePtr NJS::CallExpression::_GenIntermediate(Builder &builder, const TypePtr &expected_type)
{
    builder.PushLastObjectContext();

    const auto callee_value = Callee->GenIntermediate(builder, nullptr);
    const auto callee_type = callee_value->GetType();

    if (auto [
            callee_info,
            parameter_infos,
            is_var_arg,
            result_info,
            callee
        ] = builder.FindOperator(callee_value);
        callee)
    {
        auto extended_parameter_infos = parameter_infos;
        extended_parameter_infos.emplace(extended_parameter_infos.begin(), callee_info);
        const auto function_type = builder.GetTypeContext().GetFunctionType(
            result_info,
            extended_parameter_infos,
            is_var_arg);
        const auto parameter_count = parameter_infos.size();
        const auto argument_count = Arguments.size();

        if (argument_count < parameter_count)
            Error(Where, "not enough arguments, {} < {}", argument_count, parameter_count);
        if (argument_count > parameter_count && !is_var_arg)
            Error(Where, "too many arguments, {} > {}", argument_count, parameter_count);

        std::vector<llvm::Value *> arguments(argument_count + 1);
        arguments[0] = callee_info.SolveFor(builder, callee_value);

        for (unsigned i = 0; i < argument_count; ++i)
        {
            auto info = i < parameter_count
                            ? parameter_infos[i]
                            : ReferenceInfo();

            const auto &argument = Arguments[i];
            const auto argument_value = argument->GenIntermediate(builder, info.Type);

            arguments[i + 1] = info.SolveFor(builder, argument_value);
        }

        const auto result_value = builder.GetBuilder().CreateCall(
            function_type->GenFnLLVM(builder),
            callee,
            arguments);

        auto [
            type_,
            is_const_,
            is_reference_
        ] = result_info;
        if (is_reference_)
            return LValue::Create(builder, type_, result_value, is_const_);
        return RValue::Create(builder, type_, result_value);
    }

    if (!callee_type->IsFunction())
        Error(Where, "cannot call non-function callee of type {}", callee_type);

    const auto function_type = Type::As<FunctionType>(callee_type);
    const auto parameter_count = function_type->GetParameterCount();
    const auto argument_count = Arguments.size();

    llvm::Value *first_argument = nullptr;
    if (auto object = builder.GetLastObject();
        object &&
        (argument_count == parameter_count - 1 || (function_type->IsVarArg() && argument_count > parameter_count - 1)))
    {
        if (const auto info = function_type->GetParameter(0); info.Type == object->GetType())
            first_argument = info.SolveFor(builder, object);
    }

    builder.PopLastObjectContext();

    if (!first_argument)
    {
        if (argument_count < parameter_count)
            Error(Where, "not enough arguments, {} < {}", argument_count, parameter_count);
        if (argument_count > parameter_count && !function_type->IsVarArg())
            Error(Where, "too many arguments, {} > {}", argument_count, parameter_count);
    }

    const auto has_first = !!first_argument;
    std::vector<llvm::Value *> arguments(argument_count + has_first);

    if (has_first)
        arguments[0] = first_argument;

    for (unsigned i = has_first; i < arguments.size(); ++i)
    {
        auto info = i < parameter_count
                        ? function_type->GetParameter(i)
                        : ReferenceInfo();

        const auto &argument = Arguments[i - has_first];
        const auto argument_value = argument->GenIntermediate(builder, info.Type);

        arguments[i] = info.SolveFor(builder, argument_value);
    }

    const auto result_value = builder.GetBuilder().CreateCall(
        function_type->GenFnLLVM(builder),
        callee_value->Load(),
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
