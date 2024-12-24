#include <utility>
#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Type.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::StructExpr::StructExpr(SourceLocation where, std::map<std::string, ExprPtr> elements)
    : Expr(std::move(where)), Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::StructExpr::GenLLVM(Builder& builder)
{
    std::map<std::string, ValuePtr> elements;
    std::map<std::string, TypePtr> element_types;
    for (const auto& [name_, element_] : Elements)
    {
        const auto value = element_->GenLLVM(builder);
        elements[name_] = value;
        element_types[name_] = value->GetType();
    }
    const auto type = builder.GetCtx().GetStructType(element_types);

    llvm::Value* object = llvm::ConstantStruct::getNullValue(type->GetLLVM<llvm::StructType>(builder));

    unsigned i = 0;
    for (const auto& [name_, value_] : elements)
        object = builder.GetBuilder().CreateInsertValue(object, value_->Load(), i++);

    return RValue::Create(builder, type, object);
}

std::ostream& NJS::StructExpr::Print(std::ostream& os)
{
    if (Elements.empty()) return os << "{}";

    os << '{' << std::endl;
    Indent();
    for (const auto& [name, value] : Elements)
        value->Print(Spacing(os) << name << ": ") << ',' << std::endl;
    Exdent();
    return Spacing(os) << '}';
}
