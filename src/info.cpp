#include <NJS/Builder.hpp>
#include <NJS/Info.hpp>
#include <NJS/Parameter.hpp>
#include <NJS/Type.hpp>

bool NJS::operator==(const ReferenceInfo &a, const ReferenceInfo &b)
{
    return (a.Type == b.Type)
           && (a.IsReference == b.IsReference)
           && (a.IsReference ? a.IsConst == b.IsConst : true);
}

bool NJS::operator!=(const ReferenceInfo &a, const ReferenceInfo &b)
{
    return !(a == b);
}

bool NJS::operator<(const ReferenceInfo &a, const ReferenceInfo &b)
{
    return (a.Type < b.Type)
           && (a.IsReference < b.IsReference)
           && (a.IsConst < b.IsConst);
}

NJS::ReferenceInfo::ReferenceInfo(const TypePtr &type, const bool is_const, const bool is_reference)
    : Type(type),
      IsConst(is_const),
      IsReference(is_reference)
{
}

NJS::ReferenceInfo::ReferenceInfo(const TypePtr &type)
    : Type(type)
{
}

std::string NJS::ReferenceInfo::GetString() const
{
    return (IsReference && IsConst ? "const " : "") + Type->GetString() + (IsReference ? "&" : "");
}

llvm::Type *NJS::ReferenceInfo::GetLLVM(const SourceLocation &where, const Builder &builder) const
{
    if (IsReference)
        return builder.GetBuilder().getPtrTy();
    return Type->GetLLVM(where, builder);
}

unsigned NJS::ReferenceInfo::GetSize() const
{
    if (IsReference)
        return 8;
    return Type->GetSize();
}

std::ostream &NJS::ReferenceInfo::Print(std::ostream &stream) const
{
    if (IsReference && IsConst)
        stream << "const ";
    Type->Print(stream);
    if (IsReference)
        stream << "&";
    return stream;
}
