#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Type.hpp>

std::string NJS::PrimitiveType::GenString(const TypeName name)
{
    static std::map<TypeName, const char*> type_names
    {
        {Type_Void, "void"},
        {Type_Boolean, "boolean"},
        {Type_Number, "number"},
        {Type_String, "string"},
    };
    return type_names[name];
}

NJS::PrimitiveType::PrimitiveType(const TypeName name)
    : Type(GenString(name)), Name(name)
{
}

size_t NJS::PrimitiveType::Size()
{
    switch (Name)
    {
    case Type_Void: return 0;
    case Type_Boolean: return 1;
    case Type_Number:
    case Type_String: return 8;
    default: Error("unknown primitive type does not have any size");
    }
}

NJS::TypeId NJS::PrimitiveType::GetId() const
{
    switch (Name)
    {
    case Type_Void: return TypeId_Void;
    case Type_Boolean: return TypeId_Boolean;
    case Type_Number: return TypeId_Number;
    case Type_String: return TypeId_String;
    default: Error("unknown primitive type does not have a type id");
    }
}

llvm::Type* NJS::PrimitiveType::GenLLVM(Builder& builder) const
{
    switch (Name)
    {
    case Type_Void: return builder.LLVMBuilder().getVoidTy();
    case Type_Boolean: return builder.LLVMBuilder().getInt1Ty();
    case Type_Number: return builder.LLVMBuilder().getDoubleTy();
    case Type_String: return builder.LLVMBuilder().getPtrTy();
    default: Error("cannot generate llvm for unknown type");
    }
}
