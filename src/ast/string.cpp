#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::StringExpr::StringExpr(SourceLocation where, std::string value)
    : Expr(std::move(where)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::StringExpr::GenLLVM(Builder& builder, const TypePtr&) const
{
    const auto type = builder.GetCtx().GetStringType();
    const auto value = GetString(builder, Value);
    return RValue::Create(builder, type, value);
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
