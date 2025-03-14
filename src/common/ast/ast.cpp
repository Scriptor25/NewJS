#include <utility>
#include <newjs/ast.hpp>

NJS::Statement::Statement(SourceLocation where)
    : Where(std::move(where))
{
}

void NJS::Statement::GenLLVM(Builder &builder, const bool is_export) try
{
    PGenLLVM(builder, is_export);
}
catch (const RTError &error)
{
    Error(error, Where, {});
}

void NJS::Statement::GenImport(
    Builder &builder,
    const std::string &module_id,
    ValuePtr &dest_value,
    ReferenceInfo &dest_info,
    std::string &dest_name) try
{
    PGenImport(builder, module_id, dest_value, dest_info, dest_name);
}
catch (const RTError &error)
{
    Error(error, Where, {});
}

void NJS::Statement::PGenImport(
    Builder &builder,
    const std::string &module_id,
    ValuePtr &dest_value,
    ReferenceInfo &dest_info,
    std::string &dest_name)
{
    Error(Where, "cannot generate import variant for this statement");
}

NJS::Expression::Expression(SourceLocation where)
    : Statement(std::move(where))
{
}

NJS::ValuePtr NJS::Expression::GenLLVM(Builder &builder, const TypePtr &expected_type) try
{
    return PGenLLVM(builder, expected_type);
}
catch (const RTError &error)
{
    Error(error, Where, {});
}

void NJS::Expression::PGenLLVM(Builder &builder, bool is_export) try
{
    (void) GenLLVM(builder, nullptr);
}
catch (const RTError &error)
{
    Error(error, Where, {});
}
