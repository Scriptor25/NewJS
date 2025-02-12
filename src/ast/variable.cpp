#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Parameter.hpp>

NJS::VariableStatement::VariableStatement(
    SourceLocation where,
    const bool is_const,
    ParameterPtr name,
    ExpressionPtr value)
    : Statement(std::move(where)),
      IsConst(is_const),
      Name(std::move(name)),
      Value(std::move(value))
{
}

void NJS::VariableStatement::GenVoidLLVM(Builder &builder) const
{
    const auto value = Value ? Value->GenLLVM(builder, Name->Type) : nullptr;
    Name->CreateVars(builder, Where, IsConst, value);
}

std::ostream &NJS::VariableStatement::Print(std::ostream &os)
{
    Name->Print(os << (IsConst ? "const" : "let") << ' ');
    if (Value)
        Value->Print(os << " = ");
    return os;
}
