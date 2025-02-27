#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/parameter.hpp>

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

NJS::ValuePtr NJS::VariableStatement::GenLLVM(Builder &builder, ErrorInfo &error) const
{
    const auto value = Value
                           ? Value->GenLLVM(builder, error, Parameter->Type)
                           : nullptr;
    Parameter->CreateVars(builder, value, IsExtern, IsConst, IsReference, TODO);
    return {};
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
