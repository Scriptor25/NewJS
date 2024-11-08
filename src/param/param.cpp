#include <NJS/Builder.hpp>
#include <NJS/Context.hpp>
#include <NJS/Error.hpp>
#include <NJS/NJS.hpp>
#include <NJS/Param.hpp>
#include <NJS/Value.hpp>

NJS::Param::Param(std::string name)
    : Name(std::move(name))
{
}

bool NJS::Param::RequireValue()
{
    return false;
}

void NJS::Param::CreateVars(Builder& builder, const bool is_const, ValuePtr value)
{
    const auto type = Type ? Type : value->GetType();
    builder.CreateVar(Name) = builder.CreateGlobal(builder.ValueName(Name), type, is_const, value);
}

std::ostream& NJS::Param::Print(std::ostream& os)
{
    os << Name;
    if (Type) Type->Print(os << ": ");
    return os;
}
