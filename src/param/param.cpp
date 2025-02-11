#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Param.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::Param::Param(std::string_view name)
    : Name(std::move(name))
{
}

bool NJS::Param::RequireValue()
{
    return false;
}

void NJS::Param::CreateVars(
    Builder &builder,
    const SourceLocation &where,
    const bool is_const,
    const ValuePtr &value)
{
    const auto type = !Type
                          ? value->GetType()
                          : Type->IsRef()
                                ? Type->GetElement()
                                : Type;

    auto &var = builder.DefineVariable(where, Name);
    if (Type && Type->IsRef())
    {
        var = LValue::Create(builder, type, value->GetPtr(where));
    }
    else if (is_const)
    {
        var = RValue::Create(builder, type, value->Load(where));
    }
    else
    {
        var = builder.CreateAlloca(where, type);
        if (value)
            var->Store(where, value);
        else
            var->Store(where, llvm::Constant::getNullValue(type->GetLLVM(where, builder)));
    }
}

std::ostream &NJS::Param::Print(std::ostream &os)
{
    os << Name;
    if (Type)
        Type->Print(os << ": ");
    return os;
}
