#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::TypeOfExpr::TypeOfExpr(SourceLocation where, ExprPtr operand)
    : Expr(std::move(where)),
      Operand(std::move(operand))
{
}

NJS::ValuePtr NJS::TypeOfExpr::GenLLVM(Builder &builder, const TypePtr &expected) const
{
    const auto operand = Operand->GenLLVM(builder, {});
    const auto type = builder.GetTypeContext().GetStringType();
    const auto value = StringExpr::GetString(builder, operand->GetType()->GetString());
    return RValue::Create(builder, type, value);
}

std::ostream &NJS::TypeOfExpr::Print(std::ostream &os)
{
    return Operand->Print(os << "typeof(") << ")";
}
