#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Parameter.hpp>

NJS::VariableStatement::VariableStatement(
    SourceLocation where,
    const bool is_extern,
    const bool is_const,
    const bool is_reference,
    ParameterPtr parameter,
    ExpressionPtr value)
    : Statement(std::move(where)),
      IsExtern(is_extern),
      IsConst(is_const),
      IsReference(is_reference),
      Parameter(std::move(parameter)),
      Value(std::move(value))
{
}

void NJS::VariableStatement::GenVoidLLVM(Builder &builder) const
{
    const auto value = Value ? Value->GenLLVM(builder, Parameter->Type) : nullptr;
    Parameter->CreateVars(builder, value, IsExtern, IsConst, IsReference);
}

std::ostream &NJS::VariableStatement::Print(std::ostream &stream)
{
    if (IsExtern)
        stream << "extern ";
    stream << (IsConst ? "const " : "let ");
    if (IsReference)
        stream << "&";
    Parameter->Print(stream);
    if (Value)
        Value->Print(stream << " = ");
    return stream;
}
