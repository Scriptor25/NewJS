#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Param.hpp>

NJS::VariableStmt::VariableStmt(SourceLocation where, const bool is_const, ParamPtr name, ExprPtr value)
    : Stmt(std::move(where)), IsConst(is_const), Name(std::move(name)), Value(std::move(value))
{
}

NJS::ValuePtr NJS::VariableStmt::GenLLVM(Builder& builder)
{
    const auto value = Value ? Value->GenLLVM(builder) : nullptr;
    Name->CreateVars(builder, Where, IsConst, value);
    return {};
}

std::ostream& NJS::VariableStmt::Print(std::ostream& os)
{
    Name->Print(os << (IsConst ? "const" : "let") << ' ');
    if (Value) Value->Print(os << " = ");
    return os;
}
