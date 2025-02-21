#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Parameter.hpp>

NJS::VariableStatement::VariableStatement(
    SourceLocation where,
    ParameterPtr parameter,
    const unsigned flags,
    ExpressionPtr value)
    : Statement(std::move(where)),
      Parameter(std::move(parameter)),
      Flags(flags),
      Value(std::move(value))
{
}

void NJS::VariableStatement::GenVoidLLVM(Builder &builder) const
{
    const auto value = Value ? Value->GenLLVM(builder, Parameter->Type) : nullptr;
    Parameter->CreateVars(builder, value, Flags);
}

std::ostream &NJS::VariableStatement::Print(std::ostream &stream)
{
    if (Flags & ParameterFlags_Extern)
        stream << "extern ";
    if (Flags & ParameterFlags_Const)
        stream << "const ";
    else
        stream << "let ";

    Parameter->Print(stream);
    if (Value)
        Value->Print(stream << " = ");
    return stream;
}
