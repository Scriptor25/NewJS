#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::TupleExpr::TupleExpr(SourceLocation where, TypePtr type, std::vector<ExprPtr> elements)
    : Expr(std::move(where), std::move(type)), Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::TupleExpr::GenLLVM(Builder& builder)
{
    llvm::Value* value = llvm::Constant::getNullValue(Type->GetLLVM(builder));

    for (unsigned i = 0; i < Elements.size(); ++i)
        value = builder.GetBuilder().CreateInsertValue(value, Elements[i]->GenLLVM(builder)->Load(), i);

    return RValue::Create(builder, Type, value);
}

std::ostream& NJS::TupleExpr::Print(std::ostream& os)
{
    if (Elements.empty()) return os << "[]";

    os << '[' << std::endl;
    Indent();
    for (const auto& entry : Elements)
        entry->Print(Spacing(os)) << ',' << std::endl;
    Exdent();
    return Spacing(os) << ']';
}
