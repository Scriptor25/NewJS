#include <utility>
#include <newjs/builder.hpp>
#include <newjs/parameter.hpp>
#include <newjs/parser.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

NJS::Parameter::Parameter(
    SourceLocation where,
    std::string name,
    ReferenceInfo info)
    : Where(std::move(where)),
      Name(std::move(name)),
      Info(std::move(info))
{
}

bool NJS::Parameter::RequireValue()
{
    return Info.IsConst || Info.IsReference;
}

void NJS::Parameter::CreateVars(
    Builder &builder,
    ValuePtr value,
    const bool is_extern,
    const bool is_const,
    const bool is_reference)
{
    const auto type = Info.Type
                          ? Info.Type
                          : value->GetType();

    auto &variable = builder.DefineVariable(Name);

    if (is_extern)
    {
        const auto const_value = value
                                     ? llvm::dyn_cast<llvm::Constant>(value->Load())
                                     : nullptr;

        variable = builder.CreateGlobal(Name, type, is_const, value != nullptr, const_value);
        if (value && !const_value)
            variable->Store(value);
        return;
    }

    if (is_reference)
    {
        if (value->GetType() != type)
            Error(Where, "TODO");
        if (value->IsConst() && !is_const)
            Error(Where, "TODO");
        const auto pointer = value->GetPointer();
        variable = LValue::Create(builder, type, pointer, is_const);
        return;
    }

    if (is_const)
    {
        value = builder.CreateCast(value, type);
        const auto loaded = value->Load();
        variable = RValue::Create(builder, type, loaded);
        return;
    }

    variable = builder.CreateAlloca(type, false);
    if (value)
        variable->Store(value);
    else
        variable->Store(llvm::Constant::getNullValue(type->GetLLVM(builder)));
}

std::ostream &NJS::Parameter::Print(std::ostream &stream, const bool with_info)
{
    if (with_info && Info.IsReference)
    {
        if (Info.IsConst)
            stream << "const ";
        stream << "&";
    }
    stream << Name;
    if (Info.Type)
        Info.Type->Print(stream << ": ");
    return stream;
}
