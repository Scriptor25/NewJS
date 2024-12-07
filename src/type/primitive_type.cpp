#include <NJS/Builder.hpp>
#include <NJS/Error.hpp>
#include <NJS/Std.hpp>
#include <NJS/Type.hpp>

std::string NJS::PrimitiveType::GenString(const Primitive name)
{
    static std::map<Primitive, const char*> type_names
    {
        {Primitive_Void, "void"},
        {Primitive_Boolean, "boolean"},
        {Primitive_Number, "number"},
        {Primitive_String, "string"},
        {Primitive_Char, "char"},
    };
    return type_names[name];
}

NJS::PrimitiveType::PrimitiveType(TypeContext& ctx, const Primitive name)
    : Type(ctx, GenString(name)), Name(name)
{
}

bool NJS::PrimitiveType::IsPrimitive() const
{
    return true;
}

bool NJS::PrimitiveType::IsPrimitive(const Primitive name) const
{
    return Name == name;
}

void NJS::PrimitiveType::TypeInfo(Builder& builder, std::vector<llvm::Value*>& args) const
{
    switch (Name)
    {
    case Primitive_Void:
        args.push_back(builder.GetBuilder().getInt32(ID_VOID));
        break;
    case Primitive_Boolean:
        args.push_back(builder.GetBuilder().getInt32(ID_BOOLEAN));
        break;
    case Primitive_Number:
        args.push_back(builder.GetBuilder().getInt32(ID_NUMBER));
        break;
    case Primitive_String:
        args.push_back(builder.GetBuilder().getInt32(ID_STRING));
        break;
    case Primitive_Char:
        args.push_back(builder.GetBuilder().getInt32(ID_CHAR));
        break;
    }
}

size_t NJS::PrimitiveType::Bytes() const
{
    switch (Name)
    {
    case Primitive_Void: return 0;
    case Primitive_Boolean: return 1;
    case Primitive_Number: return 8;
    case Primitive_String: return 8;
    case Primitive_Char: return 1;
    default: return 0;
    }
}

llvm::Type* NJS::PrimitiveType::GenLLVM(Builder& builder) const
{
    switch (Name)
    {
    case Primitive_Void: return builder.GetBuilder().getVoidTy();
    case Primitive_Boolean: return builder.GetBuilder().getInt1Ty();
    case Primitive_Number: return builder.GetBuilder().getDoubleTy();
    case Primitive_String: return builder.GetBuilder().getPtrTy();
    case Primitive_Char: return builder.GetBuilder().getInt8Ty();
    default: Error("cannot generate llvm for unknown type");
    }
}
