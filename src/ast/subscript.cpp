#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::SubscriptExpr::SubscriptExpr(SourceLocation where, TypePtr type, ExprPtr array, ExprPtr index)
    : Expr(std::move(where), std::move(type)), Array(std::move(array)), Index(std::move(index))
{
}

NJS::ValuePtr NJS::SubscriptExpr::GenLLVM(Builder& builder)
{
    const auto array = Array->GenLLVM(builder);
    const auto index = Index->GenLLVM(builder);

    if (!index->GetType()->IsPrimitive(Primitive_Number))
        Error(Where, "invalid subscript: index (second operand) must be of type number, but is {}", index->GetType());

    const auto int_index = builder.GetBuilder().CreateFPToSI(index->Load(), builder.GetBuilder().getInt64Ty());
    return builder.CreateSubscript(array, int_index);
}

std::ostream& NJS::SubscriptExpr::Print(std::ostream& os)
{
    return Index->Print(Array->Print(os) << '[') << ']';
}
