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

void NJS::VariableStatement::_GenIntermediate(Builder &builder, const bool is_export)
{
    const auto value = Value
                           ? Value->GenIntermediate(builder, Parameter->Info.Type)
                           : nullptr;
    Parameter->CreateVars(builder, value, is_export, IsExtern, Parameter->Info.IsConst, Parameter->Info.IsReference);
}

void NJS::VariableStatement::_GenImport(
    Builder &builder,
    const std::string &module_id,
    ValuePtr &dest_value,
    ReferenceInfo &dest_info,
    std::string &dest_name)
{
    const auto is_const = Parameter->Info.IsConst;
    const auto type = Parameter->Info.Type;

    std::string variable_name;
    if (IsExtern)
        variable_name = Parameter->Name;
    else
        variable_name = module_id + '.' + Parameter->Name;

    dest_value = builder.CreateGlobal(variable_name, type, is_const, false);
    dest_info = {type, is_const, true};
    dest_name = Parameter->Name;
}
