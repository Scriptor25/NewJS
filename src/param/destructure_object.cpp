#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Param.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::DestructureObject::DestructureObject(std::map<std::string, ParamPtr> elements)
    : Param(""), Elements(std::move(elements))
{
}

bool NJS::DestructureObject::RequireValue()
{
    return true;
}

void NJS::DestructureObject::CreateVars(Builder& builder, const bool is_const, const ValuePtr& value)
{
    const auto type = Type ? Type : value->GetType();

    if (value->IsL())
    {
        for (const auto& [name, element] : Elements)
        {
            const auto member_type = type->Member(name);
            const auto member_index = type->MemberIndex(name);
            const auto member = LValue::Create(
                builder,
                member_type,
                builder.LLVMBuilder().CreateStructGEP(type->GenLLVM(builder), value->GetPtr(), member_index));
            element->CreateVars(builder, is_const, member);
        }
    }
    else
    {
        for (const auto& [name, element] : Elements)
        {
            const auto member_type = type->Member(name);
            const auto member_index = type->MemberIndex(name);
            const auto member = RValue::Create(
                builder,
                member_type,
                builder.LLVMBuilder().CreateExtractValue(value->Load(), member_index));
            element->CreateVars(builder, is_const, member);
        }
    }
}

std::ostream& NJS::DestructureObject::Print(std::ostream& os)
{
    os << "{ ";
    bool first = true;
    for (const auto& [name, element] : Elements)
    {
        if (first) first = false;
        else os << ", ";
        element->Print(os << name << ": ");
    }
    os << " }";
    if (Type) Type->Print(os << ": ");
    return os;
}
