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

    const auto parameter_count = callee_type->GetParameterCount();
    if (Arguments.size() < parameter_count)
        Error(Where, "not enough arguments");
    if (Arguments.size() > parameter_count && !callee_type->IsVarArg())
        Error(Where, "too many arguments");

    std::vector<llvm::Value *> argument_values(Arguments.size());
    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        const auto has_parameter = i < parameter_count;
        auto parameter_type = has_parameter
                                  ? callee_type->ParameterType(i)
                                  : nullptr;

        auto &argument = Arguments[i];
        auto argument_value = argument->GenLLVM(builder, parameter_type);

        const auto parameter_is_reference = has_parameter && parameter_type->IsReference();
        if (parameter_is_reference)
            parameter_type = parameter_type->GetElement();

        if (has_parameter)
            argument_value = builder.CreateCast(argument->Where, argument_value, parameter_type);

        argument_values[i] = parameter_is_reference
                                 ? argument_value->GetPtr(argument->Where)
                                 : argument_value->Load(argument->Where);
    }

    const auto result_value = builder.GetBuilder().CreateCall(
        callee_type->GenFnLLVM(Where, builder),
        callee->Load(Where),
        argument_values);

    if (callee_type->GetResultType()->IsReference())
        return LValue::Create(builder, callee_type->GetResultType()->GetElement(), result_value);

    return RValue::Create(builder, callee_type->GetResultType(), result_value);
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
