#include <utility>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::Parameter::Parameter(std::string name)
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
    const ValuePtr &value,
    const unsigned flags)
{
    const auto type = !Type
                          ? value->GetType()
                          : Type->IsReference()
                                ? Type->GetElement()
                                : Type;

    auto &variable = builder.DefineVariable(where, Name);

    const bool is_extern = flags & ParameterFlags_Extern;
    const bool is_const = flags & ParameterFlags_Const;

    if (is_extern)
    {
        variable = builder.CreateGlobal(where, Name, type, false);
        return;
    }

    if (Type && Type->IsReference())
    {
        variable = LValue::Create(builder, type, value->GetPtr(where));
        return;
    }

    if (is_const)
    {
        variable = RValue::Create(builder, type, value->Load(where));
        return;
    }

    variable = builder.CreateAlloca(where, type);
    if (value)
    {
        variable->Store(where, value);
        return;
    }

    variable->Store(where, llvm::Constant::getNullValue(type->GetLLVM(where, builder)));
}

std::ostream &NJS::Parameter::Print(std::ostream &stream)
{
    stream << Name;
    if (Type)
        Type->Print(stream << ": ");
    return stream;
}
