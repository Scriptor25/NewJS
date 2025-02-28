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
    ValuePtr variable;

    if (is_extern)
    {
        const auto const_value = value
                                     ? llvm::dyn_cast<llvm::Constant>(value->Load())
                                     : nullptr;

        variable = builder.CreateGlobal(Name, type, is_const, value != nullptr, const_value);
        if (value && !const_value)
            variable->Store(value);
    }
    else if (is_reference)
    {
        if (value->GetType() != type)
            return;
        if (value->IsConst() && !is_const)
            return;
        const auto pointer = value->GetPointer();
        variable = LValue::Create(builder, type, pointer, is_const);
    }
    else if (is_const)
    {
        value = builder.CreateCast(value, type);
        const auto loaded = value->Load();
        variable = RValue::Create(builder, type, loaded);
    }
    else
    {
        variable = builder.CreateAlloca(type, false);
        if (value)
            variable->Store(value);
        else
            variable->Store(llvm::Constant::getNullValue(type->GetLLVM(builder)));
    }

    builder.DefineVariable(Name, variable);
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
