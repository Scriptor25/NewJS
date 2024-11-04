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
    std::map<std::string, ValuePtr> values;
    std::map<std::string, TypePtr> types;

    for (const auto& [name, element] : Elements)
    {
        const auto value = element->GenLLVM(builder);
        values[name] = value;
        types[name] = value->GetType();
    }

    const auto type = builder.Ctx().GetObjectType(types);
    const auto llvm_type = type->GenLLVM(builder);

    const auto object = builder.CreateAlloca(type);
    const auto ptr = object->GetPtr();

    size_t i = 0;
    for (const auto& [name, value] : values)
    {
        const auto gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, ptr, i++);
        builder.LLVMBuilder().CreateStore(value->Load(), gep);
    }

    return object;
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
