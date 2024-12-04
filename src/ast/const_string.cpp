#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Value.hpp>

NJS::ConstStringExpr::ConstStringExpr(std::string value)
    : Value(std::move(value))
{
}

NJS::ValuePtr NJS::ConstStringExpr::GenLLVM(Builder& builder)
{
    const auto ptr = GetString(builder, Value);
    return RValue::Create(builder, builder.Ctx().GetStringType(), ptr);
}

std::ostream& NJS::ConstStringExpr::Print(std::ostream& os)
{
    return os << '"' << Value << '"';
}

llvm::Constant* NJS::ConstStringExpr::GetString(Builder& builder, const std::string& value)
{
    static std::map<std::string, llvm::Constant*> string_table;

    auto& ptr = string_table[value];
    if (!ptr) ptr = builder.LLVMBuilder().CreateGlobalStringPtr(value);
    return ptr;
}
