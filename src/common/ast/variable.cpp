#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/parameter.hpp>

NJS::VariableStatement::VariableStatement(
    SourceLocation where,
    const bool is_extern,
    ParameterPtr parameter,
    ExpressionPtr value)
    : Statement(std::move(where)),
      IsExtern(is_extern),
      Parameter(std::move(parameter)),
      Value(std::move(value))
{
}

void NJS::VariableStatement::PGenLLVM(Builder &builder) const
{
    const auto value = Value
                           ? Value->GenLLVM(builder, Parameter->Info.Type)
                           : nullptr;
    Parameter->CreateVars(builder, value, IsExtern, Parameter->Info.IsConst, Parameter->Info.IsReference);
}

std::ostream &NJS::VariableStatement::Print(std::ostream &stream) const
{
    if (IsExtern)
        stream << "extern ";
    stream << (Parameter->Info.IsConst ? "const " : "let ");
    if (Parameter->Info.IsReference)
        stream << "&";
    Parameter->Print(stream, false);
    if (Value)
        Value->Print(stream << " = ");
    return stream;
}
