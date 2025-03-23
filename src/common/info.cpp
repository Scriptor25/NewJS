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
           && (!a.IsReference || a.IsConst == b.IsConst);
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

size_t NJS::ReferenceInfo::GetHash() const
{
    return CombineHashes(CombineHashes(std::hash<bool>()(IsReference), std::hash<bool>()(IsConst)), Type->GetHash());
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
            Error("cannot create reference of type {} to value of type {}", Type, value->GetType());
        if (!IsConst && value->IsConst())
            Error("cannot create mutable reference to constant value");
        if (!value->IsLValue())
        {
            const auto alloc = builder.CreateAlloca(Type, true);
            alloc->StoreNoError(value);
            value = alloc;
        }
        return value->GetPointer();
    }

    if (Type)
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
