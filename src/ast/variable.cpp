#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Parameter.hpp>

NJS::VariableStatement::VariableStatement(
    SourceLocation where,
    ParameterPtr name,
    const unsigned flags,
    ExpressionPtr value)
    : Statement(std::move(where)),
      Name(std::move(name)),
      Flags(flags),
      Value(std::move(value))
{
}

void NJS::VariableStatement::GenVoidLLVM(Builder &builder) const
{
    const auto value = Value ? Value->GenLLVM(builder, Name->Type) : nullptr;
    Name->CreateVars(builder, Where, value, Flags);
}

std::ostream &NJS::VariableStatement::Print(std::ostream &stream)
{
    if (Flags & ParameterFlags_Extern)
        stream << "extern ";
    if (Flags & ParameterFlags_Const)
        stream << "const ";
    else
        stream << "let ";

    Name->Print(stream);
    if (Value)
        Value->Print(stream << " = ");
    return stream;
}
