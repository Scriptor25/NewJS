#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Value.hpp>

NJS::ConstNumberExpr::ConstNumberExpr(const double value)
    : Value(value)
{
}

NJS::ValuePtr NJS::ConstNumberExpr::GenLLVM(Builder& builder)
{
    const auto type = builder.Ctx().GetNumberType();
    const auto value = llvm::ConstantFP::get(builder.LLVMBuilder().getDoubleTy(), Value);
    return RValue::Create(builder, type, value);
}

std::ostream& NJS::ConstNumberExpr::Print(std::ostream& os)
{
    return os << Value;
}
