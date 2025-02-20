#include <utility>
#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Parser.hpp>
#include <NJS/Type.hpp>
#include <NJS/Value.hpp>

NJS::Parameter::Parameter(SourceLocation where, std::string name)
    : Where(std::move(where)),
      Name(std::move(name))
{
}

bool NJS::Parameter::RequireValue()
{
    return false;
}

void NJS::Parameter::CreateVars(
    Builder &builder,
    ValuePtr value,
    const unsigned flags)
{
    const auto type = !Type
                          ? value->GetType()
                          : Type->IsReference()
                                ? Type->GetElement(Where)
                                : Type;

    auto &variable = builder.DefineVariable(Where, Name);

    const bool is_extern = flags & ParameterFlags_Extern;
    const bool is_const = flags & ParameterFlags_Const;

    if (is_extern)
    {
        variable = builder.CreateGlobal(Where, Name, type, false);
        return;
    }

    if (Type && Type->IsReference())
    {
        if (value->GetType() != type)
            Error(
                Where,
                "type mismatch: cannot create reference with type {} from value of type {}",
                type,
                value->GetType());
        variable = LValue::Create(builder, type, value->GetPtr(Where));
        return;
    }

    if (is_const)
    {
        value = builder.CreateCast(Where, value, type);
        variable = RValue::Create(builder, type, value->Load(Where));
        return;
    }

    variable = builder.CreateAlloca(Where, type);
    if (value)
    {
        variable->Store(Where, value);
        return;
    }

    variable->Store(Where, llvm::Constant::getNullValue(type->GetLLVM(Where, builder)));
}

std::ostream &NJS::Parameter::Print(std::ostream &stream)
{
    stream << Name;
    if (Type)
        Type->Print(stream << ": ");
    return stream;
}
