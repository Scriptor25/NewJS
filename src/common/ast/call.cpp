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

        if (const auto info = function_type->GetParameter(0); info.Type == object->GetType())
            first_argument = info.SolveFor(builder, object);
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
        auto info = i < parameter_count
                        ? function_type->GetParameter(i)
                        : ReferenceInfo();

        auto &argument = Arguments[i - has_first];
        const auto argument_value = argument->GenLLVM(builder, info.Type);

        arguments[i] = info.SolveFor(builder, argument_value);
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
