#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::ConstObjectExpr::ConstObjectExpr(std::map<std::string, ExprPtr> elements)
    : Elements(std::move(elements))
{
}

NJS::ValuePtr NJS::ConstObjectExpr::GenLLVM(Builder& builder)
{
    std::vector<ValuePtr> values;
    std::map<std::string, TypePtr> types;

    for (const auto& [name, element] : Elements)
    {
        const auto value = element->GenLLVM(builder);
        values.push_back(value);
        types[name] = value->GetType();
    }

    const auto type = builder.Ctx().GetObjectType(types);

    llvm::Value* object = llvm::ConstantStruct::getNullValue(type->GenLLVM(builder));

    for (size_t i = 0; i < values.size(); ++i)
        object = builder.LLVMBuilder().CreateInsertValue(object, values[i]->Load(), {1, i});

    return RValue::Create(builder, type, object);
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
