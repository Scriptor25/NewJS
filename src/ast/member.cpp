#include <NJS/AST.hpp>
#include <NJS/Value.hpp>

#include "NJS/Builder.hpp"
#include "NJS/Type.hpp"

NJS::MemberExpr::MemberExpr(ExprPtr object, std::string member)
    : Object(std::move(object)), Member(std::move(member))
{
}

NJS::ValuePtr NJS::MemberExpr::GenLLVM(Builder& builder)
{
    const auto object = Object->GenLLVM(builder);

    const auto llvm_type = object->GetLLVMType();
    const auto llvm_ptr = object->GetPtr();

    const auto type = object->GetType();
    const auto member_type = type->Member(Member);
    const auto member_index = type->MemberIndex(Member);

    const auto llvm_gep = builder.LLVMBuilder().CreateStructGEP(llvm_type, llvm_ptr, member_index, Member);
    return LValue::Create(builder, type, llvm_gep);
}

std::ostream& NJS::MemberExpr::Print(std::ostream& os)
{
    return Object->Print(os) << '.' << Member;
}
