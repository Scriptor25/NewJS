#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::StringExpr::StringExpr(SourceLocation where, TypePtr type, std::string value)
    : Expr(std::move(where), std::move(type)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::StringExpr::GenLLVM(Builder& builder)
{
    const auto ptr = GetString(builder, Value);
    return RValue::Create(builder, Type, ptr);
}

std::ostream& NJS::StringExpr::Print(std::ostream& os)
{
    return os << '"' << Value << '"';
}

llvm::Constant* NJS::StringExpr::GetString(const Builder& builder, const std::string& value)
{
    static std::map<std::string, llvm::Constant*> string_table;

    auto& ptr = string_table[value];
    if (!ptr) ptr = builder.GetBuilder().CreateGlobalStringPtr(value);
    return ptr;
}
