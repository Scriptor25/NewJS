#include <utility>
#include <newjs/ast.hpp>
#include <newjs/builder.hpp>
#include <newjs/type.hpp>
#include <newjs/type_context.hpp>
#include <newjs/value.hpp>

llvm::Constant *NJS::StringExpression::GetString(const Builder &builder, const std::string &value)
{
    static std::map<std::string, std::map<std::string, llvm::Constant *>> string_table;

    auto &ptr = string_table[builder.GetModuleID()][value];
    if (!ptr)
        ptr = builder.GetBuilder().CreateGlobalStringPtr(value);
    return ptr;
}

NJS::StringExpression::StringExpression(SourceLocation where, std::string value)
    : Expression(std::move(where)),
      Value(std::move(value))
{
}

NJS::ValuePtr NJS::StringExpression::GenLLVM(Builder &builder, const TypePtr &) const
{
    const auto type = builder.GetTypeContext().GetStringType();
    const auto value = GetString(builder, Value);
    return RValue::Create(builder, type, value);
}

std::ostream &NJS::StringExpression::Print(std::ostream &stream)
{
    return stream << '"' << Value << '"';
}
