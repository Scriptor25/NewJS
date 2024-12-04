#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
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

bool NJS::PrimitiveType::IsPrimitive() const
{
    return true;
}

size_t NJS::PrimitiveType::Size() const
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

void NJS::PrimitiveType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    switch (Name)
    {
    case Type_Void:
        args.push_back(builder.LLVMBuilder().getInt32(ID_VOID));
        break;
    case Type_Boolean:
        args.push_back(builder.LLVMBuilder().getInt32(ID_BOOLEAN));
        break;
    case Type_Number:
        args.push_back(builder.LLVMBuilder().getInt32(ID_NUMBER));
        break;
    case Type_String:
        args.push_back(builder.LLVMBuilder().getInt32(ID_STRING));
        break;
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
