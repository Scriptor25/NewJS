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

    if (object->IsL())
    {
        const auto ty = type->GenLLVM(builder);
        const auto ptr = object->GetPtr();
        const auto gep = builder.LLVMBuilder().CreateStructGEP(ty, ptr, member_index);
        return LValue::Create(builder, member_type, gep);
    }

    const auto value = builder.LLVMBuilder().CreateExtractValue(object->Load(), member_index);
    return RValue::Create(builder, member_type, value);
}

std::ostream& NJS::MemberExpr::Print(std::ostream& os)
{
    return Object->Print(os) << '.' << Member;
}
