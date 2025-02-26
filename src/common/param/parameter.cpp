#include <utility>
#include <newjs/builder.hpp>
#include <newjs/error.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::Parameter::Parameter(
    SourceLocation where,
    std::string name,
    TypePtr type,
    ReferenceInfo info)
    : Where(std::move(where)),
      Name(std::move(name)),
      Type(std::move(type)),
      Info(std::move(info))
{
}

bool NJS::Parameter::RequireValue()
{
    return false;
}

void NJS::Parameter::CreateVars(
    Builder &builder,
    ValuePtr value,
    const bool is_extern,
    const bool is_const,
    const bool is_reference)
{
    const auto type = Type ? Type : value->GetType();
    auto &variable = builder.DefineVariable(Where, Name);

    if (is_extern)
    {
        const auto const_value = value ? llvm::dyn_cast<llvm::Constant>(value->Load(Where)) : nullptr;
        variable = builder.CreateGlobal(Where, Name, type, is_const, value != nullptr, const_value);
        if (value && !const_value)
            variable->Store(Where, value);
        return;
    }

    if (is_reference)
    {
        if (value->GetType() != type)
            Error(
                Where,
                "type mismatch: cannot create reference with type {} from value of type {}",
                type,
                value->GetType());
        if (value->IsConst() && !is_const)
            Error(Where, "cannot reference constant value as mutable");
        variable = LValue::Create(builder, type, value->GetPtr(Where), is_const);
        return;
    }

    if (is_const)
    {
        value = builder.CreateCast(Where, value, type);
        variable = RValue::Create(builder, type, value->Load(Where));
        return;
    }

    variable = builder.CreateAlloca(Where, type, false);
    if (value)
    {
        variable->Store(Where, value);
        return;
    }

    variable->Store(Where, llvm::Constant::getNullValue(type->GetLLVM(Where, builder)));
}

std::ostream &NJS::Parameter::Print(std::ostream &stream)
{
    if (Info.IsReference)
    {
        if (Info.IsConst)
            stream << "const ";
        stream << "&";
    }
    stream << Name;
    if (Type)
        Type->Print(stream << ": ");
    return stream;
}
