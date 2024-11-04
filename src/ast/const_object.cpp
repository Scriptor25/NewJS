#include <NJS/AST.hpp>
#include <NJS/Builder.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Value.hpp>

NJS::ConstObjectExpr::ConstObjectExpr(TypePtr type, std::map<std::string, ExprPtr> entries)
    : Expr(std::move(type)), Entries(std::move(entries))
{
}

NJS::ValuePtr NJS::ConstObjectExpr::GenLLVM(Builder& builder)
{
    const auto object = builder.CreateAlloca(Type);
    const auto llvm_type = Type->GenLLVM(builder);

    for (const auto& [name, value] : Entries)
    {
        const auto ptr = builder.LLVMBuilder().CreateStructGEP(llvm_type, object->GetPtr(), Type->MemberIndex(name));
        const auto val = value->GenLLVM(builder);
        builder.LLVMBuilder().CreateStore(val->Load(), ptr);
    }

    return object;
}

std::ostream& NJS::ConstObjectExpr::Print(std::ostream& os)
{
    if (Entries.empty()) return os << "{}";

    os << '{' << std::endl;
    Indent();
    for (const auto& [name, value] : Entries)
        Spacing(os) << name << ": " << value << ',' << std::endl;
    Exdent();
    return Spacing(os) << '}';
}
