#include <utility>
#include <newjs/builder.hpp>
#include <newjs/info.hpp>
#include <newjs/parameter.hpp>
#include <newjs/type.hpp>
#include <newjs/value.hpp>

bool NJS::operator==(const ReferenceInfo &a, const ReferenceInfo &b)
{
    return a.Type == b.Type
           && a.IsReference == b.IsReference
           && (a.IsReference ? a.IsConst == b.IsConst : true);
}

bool NJS::operator!=(const ReferenceInfo &a, const ReferenceInfo &b)
{
    return !(a == b);
}

NJS::ReferenceInfo::ReferenceInfo(TypePtr type, const bool is_const, const bool is_reference)
    : Type(std::move(type)),
      IsConst(is_const),
      IsReference(is_reference)
{
}

NJS::ReferenceInfo::ReferenceInfo(TypePtr type)
    : Type(std::move(type))
{
}

std::string NJS::ReferenceInfo::GetString() const
{
    return (IsReference ? IsConst ? "const &" : "&" : "") + Type->GetString();
}

llvm::Type *NJS::ReferenceInfo::GetLLVM(const Builder &builder) const
{
    if (IsReference)
        return builder.GetBuilder().getPtrTy();
    return Type->GetLLVM(builder);
}

llvm::Value *NJS::ReferenceInfo::SolveFor(const Builder &builder, ValuePtr value) const
{
    if (IsReference)
    {
        if (Type != value->GetType())
            Error("TODO");
        if (!IsConst && value->IsConst())
            Error("TODO");
        if (!value->IsLValue())
        {
            const auto alloc = builder.CreateAlloca(Type, true);
            alloc->StoreNoError(value);
            value = alloc;
        }
        return value->GetPointer();
    }

    value = builder.CreateCast(value, Type);
    return value->Load();
}

std::ostream &NJS::ReferenceInfo::Print(std::ostream &stream) const
{
    if (IsReference)
    {
        if (IsConst)
            stream << "const ";
        stream << "&";
    }
    return Type->Print(stream);
}
