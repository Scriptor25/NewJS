#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/TypeContext.hpp>
#include <NJS/Value.hpp>

NJS::Param::Param(std::string name)
    : Name(std::move(name))
{
}

bool NJS::Param::RequireValue()
{
    return false;
}

void NJS::Param::CreateVars(
    Builder& builder,
    const SourceLocation& where,
    const bool is_const,
    const ValuePtr& value)
{
    const auto type = Type
                          ? Type->IsRef()
                                ? Type->GetElement()
                                : Type
                          : value->GetType();

    auto& var = builder.DefVar(where, Name);
    if (Type && Type->IsRef())
        var = value;
    else
    {
        var = builder.CreateAlloca(type);
        if (value) var->Store(value);
        else var->Store(llvm::Constant::getNullValue(type->GetLLVM(builder)));
    }
}

std::ostream& NJS::Param::Print(std::ostream& os)
{
    os << Name;
    if (Type) Type->Print(os << ": ");
    return os;
}
