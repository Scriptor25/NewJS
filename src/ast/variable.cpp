#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Param.hpp>

NJS::VariableStmt::VariableStmt(
    SourceLocation where,
    const bool is_const,
    ParamPtr name,
    ExprPtr value)
    : Stmt(std::move(where)),
      IsConst(is_const),
      Name(std::move(name)),
      Value(std::move(value))
{
}

void NJS::VariableStmt::GenVoidLLVM(Builder& builder) const
{
    const auto value = Value ? Value->GenLLVM(builder, Name->Type) : nullptr;
    Name->CreateVars(builder, Where, IsConst, value);
}

std::ostream& NJS::VariableStmt::Print(std::ostream& os)
{
    Name->Print(os << (IsConst ? "const" : "let") << ' ');
    if (Value) Value->Print(os << " = ");
    return os;
}
