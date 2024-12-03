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
    const auto type = object->GetType();

    const auto member_type = type->Member(Member);
    const auto member_index = type->MemberIndex(Member);

    const auto ty = type->GenLLVM(builder);
    const auto ptr = object->GetPtr();
    const auto data_ty = type->GenBaseLLVM(builder);
    const auto data_ptr = builder.LLVMBuilder().CreateStructGEP(ty, ptr, 1);

    const auto gep = builder.LLVMBuilder().CreateStructGEP(data_ty, data_ptr, member_index, Member);
    return LValue::Create(builder, member_type, gep);
}

std::ostream& NJS::MemberExpr::Print(std::ostream& os)
{
    return Object->Print(os) << '.' << Member;
}
