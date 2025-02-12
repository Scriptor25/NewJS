#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::Parameter::Parameter(std::string_view name)
    : Name(std::move(name))
{
}

bool NJS::Parameter::RequireValue()
{
    return false;
}

void NJS::Parameter::CreateVars(
    Builder &builder,
    const SourceLocation &where,
    const bool is_const,
    const ValuePtr &value)
{
    const auto type = !Type
                          ? value->GetType()
                          : Type->IsReference()
                                ? Type->GetElement()
                                : Type;

    auto &var = builder.DefineVariable(where, Name);
    if (Type && Type->IsReference())
    {
        var = LValue::Create(builder, type, value->GetPtr(where));
        return;
    }

    if (is_const)
    {
        var = RValue::Create(builder, type, value->Load(where));
        return;
    }

    var = builder.CreateAlloca(where, type);
    if (value)
    {
        var->Store(where, value);
        return;
    }

    var->Store(where, llvm::Constant::getNullValue(type->GetLLVM(where, builder)));
}

std::ostream &NJS::Parameter::Print(std::ostream &stream)
{
    stream << Name;
    if (Type)
        Type->Print(stream << ": ");
    return stream;
}
