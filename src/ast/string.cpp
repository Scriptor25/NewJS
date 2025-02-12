#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

llvm::Constant *NJS::StringExpression::GetString(const Builder &builder, const std::string_view &value)
{
    static std::map<std::string, llvm::Constant *> string_table;

    auto &ptr = string_table[std::string(value)];
    if (!ptr)
        ptr = builder.GetBuilder().CreateGlobalStringPtr(value);
    return ptr;
}

NJS::StringExpression::StringExpression(SourceLocation where, std::string_view value)
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

std::ostream &NJS::StringExpression::Print(std::ostream &os)
{
    return os << '"' << Value << '"';
}
