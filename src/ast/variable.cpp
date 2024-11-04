#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Param.hpp>

NJS::VariableStmt::VariableStmt(const bool is_const, ParamPtr name, ExprPtr value)
    : IsConst(is_const), Name(std::move(name)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::VariableStmt::GenLLVM(Builder& builder)
{
    const auto value = Value ? Value->GenLLVM(builder) : nullptr;
    Name->CreateVars(builder, IsConst, value);
    return {};
}

std::ostream& NJS::VariableStmt::Print(std::ostream& os)
{
    os << (IsConst ? "const" : "let") << ' ' << Name;
    if (Value) os << " = " << Value;
    return os;
}
