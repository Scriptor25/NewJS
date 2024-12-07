#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ConstTupleExpr::ConstTupleExpr(SourceLocation where, TypePtr type, std::vector<ExprPtr> elements)
    : Expr(std::move(where), std::move(type)), Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::ConstTupleExpr::GenLLVM(Builder& builder)
{
    llvm::Value* value = llvm::Constant::getNullValue(Type->GenLLVM(builder));

    for (size_t i = 0; i < Elements.size(); ++i)
        value = builder.GetBuilder().CreateInsertValue(value, Elements[i]->GenLLVM(builder)->Load(), i);

    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::ConstTupleExpr::Print(std::ostream& os)
{
    if (Elements.empty()) return os << "[]";

    os << '[' << std::endl;
    Indent();
    for (const auto& entry : Elements)
        entry->Print(Spacing(os)) << ',' << std::endl;
    Exdent();
    return Spacing(os) << ']';
}
