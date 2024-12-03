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
    auto& ptr = GlobalStringTable[Value];
    if (!ptr) ptr = builder.LLVMBuilder().CreateGlobalStringPtr(Value);
    return RValue::Create(builder, builder.Ctx().GetStringType(), ptr);
}

std::ostream& NJS::ConstStringExpr::Print(std::ostream& os)
{
    return os << '"' << Value << '"';
}

std::map<std::string, llvm::Constant*> NJS::ConstStringExpr::GlobalStringTable;
