#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Value.hpp>

NJS::ConstStringExpr::ConstStringExpr(SourceLocation where, TypePtr type, std::string value)
    : Expr(std::move(where), std::move(type)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::ConstStringExpr::GenLLVM(Builder& builder)
{
    const auto ptr = GetString(builder, Value);
    return RValue::Create(builder, Type, ptr);
}

std::ostream& NJS::ConstStringExpr::Print(std::ostream& os)
{
    return os << '"' << Value << '"';
}

llvm::Constant* NJS::ConstStringExpr::GetString(const Builder& builder, const std::string& value)
{
    static std::map<std::string, llvm::Constant*> string_table;

    auto& ptr = string_table[value];
    if (!ptr) ptr = builder.GetBuilder().CreateGlobalStringPtr(value);
    return ptr;
}
