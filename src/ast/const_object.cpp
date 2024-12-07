#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ConstObjectExpr::ConstObjectExpr(SourceLocation where, TypePtr type, std::map<std::string, ExprPtr> elements)
    : Expr(std::move(where), std::move(type)), Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::ConstObjectExpr::GenLLVM(Builder& builder)
{
    llvm::Value* object = llvm::ConstantStruct::getNullValue(Type->GenLLVM(builder));

    size_t i = 0;
    for (const auto& [name_, element_] : Elements)
        object = builder.GetBuilder().CreateInsertValue(object, element_->GenLLVM(builder)->Load(), i++);

    return RValue::Create(builder, Type, object);
}

std::ostream& NJS::ConstObjectExpr::Print(std::ostream& os)
{
    if (Elements.empty()) return os << "{}";

    os << '{' << std::endl;
    Indent();
    for (const auto& [name, value] : Elements)
        value->Print(Spacing(os) << name << ": ") << ',' << std::endl;
    Exdent();
    return Spacing(os) << '}';
}
