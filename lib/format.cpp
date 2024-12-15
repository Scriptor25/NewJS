#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <NJS/Std.hpp>

void* operator new(const size_t size) noexcept
{
    return malloc(size);
}

void operator delete(void* block) noexcept
{
    free(block);
}

void operator delete(void* ptr, size_t) noexcept
{
    free(ptr);
}

template <typename T>
static T* New(const size_t count)
{
    const auto ptr = malloc(count * sizeof(T));
    return static_cast<T*>(ptr);
}

template <typename A, typename B>
struct Pair
{
    A first;
    B second;
};

struct Type;
struct VoidType;
struct IntType;
struct FPType;
struct PointerType;
struct ArrayType;
struct StructType;
struct TupleType;
struct FunctionType;

static void Void_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void Void_AppendP(Type*, char*, unsigned, unsigned&, char*&);
static void Int_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void Int_AppendP(Type*, char*, unsigned, unsigned&, char*&);
static void FP_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void FP_AppendP(Type*, char*, unsigned, unsigned&, char*&);
static void Pointer_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void Pointer_AppendP(Type*, char*, unsigned, unsigned&, char*&);
static void Array_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void Array_AppendP(Type*, char*, unsigned, unsigned&, char*&);
static void Struct_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void Struct_AppendP(Type*, char*, unsigned, unsigned&, char*&);
static void Tuple_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void Tuple_AppendP(Type*, char*, unsigned, unsigned&, char*&);
static void Function_AppendV(Type*, char*, unsigned, unsigned&, va_list&);
static void Function_AppendP(Type*, char*, unsigned, unsigned&, char*&);

struct Type
{
    using AppendVProc = void(*)(Type*, char*, unsigned, unsigned&, va_list&);
    using AppendPProc = void(*)(Type*, char*, unsigned, unsigned&, char*&);

    Type(const AppendVProc append_v, const AppendPProc append_p)
        : AppendV(append_v), AppendP(append_p)
    {
    }

    void Append(char* stream, const unsigned n, unsigned& offset, va_list& ap)
    {
        AppendV(this, stream, n, offset, ap);
    }

    void AppendPtr(char* stream, const unsigned n, unsigned& offset, char*& ptr)
    {
        AppendP(this, stream, n, offset, ptr);
    }

    AppendVProc AppendV;
    AppendPProc AppendP;
};

struct VoidType : Type
{
    VoidType()
        : Type(Void_AppendV, Void_AppendP)
    {
    }
};

struct IntType : Type
{
    IntType(const unsigned bits, const bool is_signed)
        : Type(Int_AppendV, Int_AppendP), Bits(bits), IsSigned(is_signed)
    {
    }

    unsigned Bits;
    bool IsSigned;
};

struct FPType : Type
{
    explicit FPType(const unsigned bits)
        : Type(FP_AppendV, FP_AppendP), Bits(bits)
    {
    }

    unsigned Bits;
};

struct PointerType : Type
{
    explicit PointerType(Type* element)
        : Type(Pointer_AppendV, Pointer_AppendP), Element(element)
    {
    }

    Type* Element;
};

struct ArrayType : Type
{
    ArrayType(Type* element, const unsigned element_count)
        : Type(Array_AppendV, Array_AppendP), Element(element), ElementCount(element_count)
    {
    }

    ~ArrayType()
    {
        delete Element;
    }

    Type* Element;
    unsigned ElementCount;
};

struct TupleType : Type
{
    explicit TupleType(Type** elements, const unsigned element_count)
        : Type(Tuple_AppendV, Tuple_AppendP), Elements(elements), ElementCount(element_count)
    {
    }

    ~TupleType()
    {
        for (unsigned i = 0; i < ElementCount; ++i)
            delete Elements[i];
        delete[] Elements;
    }

    Type** Elements;
    unsigned ElementCount;
};

struct StructType : Type
{
    explicit StructType(Pair<const char*, Type*>* elements, const unsigned element_count)
        : Type(Struct_AppendV, Struct_AppendP), Elements(elements), ElementCount(element_count)
    {
    }

    ~StructType()
    {
        for (unsigned i = 0; i < ElementCount; ++i)
            delete Elements[i].second;
        delete[] Elements;
    }

    Pair<const char*, Type*>* Elements;
    unsigned ElementCount;
};

struct FunctionType : Type
{
    FunctionType(Type* result, Type** args, const unsigned arg_count, const bool vararg)
        : Type(Function_AppendV, Function_AppendP),
          Result(result),
          Args(args),
          ArgCount(arg_count),
          VarArg(vararg)
    {
    }

    ~FunctionType()
    {
        delete Result;
        for (unsigned i = 0; i < ArgCount; ++i)
            delete Args[i];
        delete[] Args;
    }

    Type* Result;
    Type** Args;
    unsigned ArgCount;
    bool VarArg;
};

Type* ParseType(va_list& ap)
{
    switch (va_arg(ap, unsigned))
    {
    case ID_VOID:
        return new VoidType();

    case ID_INT:
        {
            const auto bits = va_arg(ap, unsigned);
            const auto is_signed = va_arg(ap, int);
            return new IntType(bits, is_signed);
        }

    case ID_FP:
        {
            const auto bits = va_arg(ap, unsigned);
            return new FPType(bits);
        }

    case ID_POINTER:
        {
            const auto element = ParseType(ap);
            return new PointerType(element);
        }

    case ID_ARRAY:
        {
            const auto element_count = va_arg(ap, unsigned);
            const auto element = ParseType(ap);
            return new ArrayType(element, element_count);
        }

    case ID_STRUCT:
        {
            const auto element_count = va_arg(ap, unsigned);
            const auto elements = New<Pair<const char*, Type*>>(element_count);
            for (unsigned i = 0; i < element_count; ++i)
            {
                const auto name = va_arg(ap, const char*);
                const auto type = ParseType(ap);
                elements[i] = {name, type};
            }
            return new StructType(elements, element_count);
        }

    case ID_TUPLE:
        {
            const auto element_count = va_arg(ap, unsigned);
            const auto elements = New<Type*>(element_count);
            for (unsigned i = 0; i < element_count; ++i)
                elements[i] = ParseType(ap);
            return new TupleType(elements, element_count);
        }

    case ID_FUNCTION:
        {
            const auto result = ParseType(ap);
            const auto arg_count = va_arg(ap, unsigned);
            const auto args = New<Type*>(arg_count);
            for (unsigned i = 0; i < arg_count; ++i)
                args[i] = ParseType(ap);
            const auto vararg = va_arg(ap, int);
            return new FunctionType(result, args, arg_count, vararg);
        }

    default:
        return {};
    }
}

void Void_AppendV(Type*, char* stream, const unsigned n, unsigned& offset, va_list&)
{
    offset += snprintf(stream + offset, n - offset, "void");
}

void Void_AppendP(Type*, char* stream, const unsigned n, unsigned& offset, char*&)
{
    offset += snprintf(stream + offset, n - offset, "void");
}

void Int_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = static_cast<IntType*>(type);
    switch (self->Bits)
    {
    case 8:
        if (self->IsSigned)
        {
            const auto val = va_arg(ap, int8_t);
            offset += snprintf(stream + offset, n - offset, "%hhi", val);
        }
        else
        {
            const auto val = va_arg(ap, uint8_t);
            offset += snprintf(stream + offset, n - offset, "%hhu", val);
        }
        break;
    case 16:
        if (self->IsSigned)
        {
            const auto val = va_arg(ap, int16_t);
            offset += snprintf(stream + offset, n - offset, "%hi", val);
        }
        else
        {
            const auto val = va_arg(ap, uint16_t);
            offset += snprintf(stream + offset, n - offset, "%hu", val);
        }
        break;
    case 32:
        if (self->IsSigned)
        {
            const auto val = va_arg(ap, int32_t);
            offset += snprintf(stream + offset, n - offset, "%li", val);
        }
        else
        {
            const auto val = va_arg(ap, uint32_t);
            offset += snprintf(stream + offset, n - offset, "%lu", val);
        }
        break;
    case 64:
        if (self->IsSigned)
        {
            const auto val = va_arg(ap, int64_t);
            offset += snprintf(stream + offset, n - offset, "%lli", val);
        }
        else
        {
            const auto val = va_arg(ap, uint64_t);
            offset += snprintf(stream + offset, n - offset, "%llu", val);
        }
        break;
    default: break;
    }
}

void Int_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = static_cast<IntType*>(type);
    switch (self->Bits)
    {
    case 8:
        if (self->IsSigned)
        {
            const auto val = *reinterpret_cast<int8_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%hhi", val);
        }
        else
        {
            const auto val = *reinterpret_cast<uint8_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%hhu", val);
        }
        ptr += sizeof(int8_t);
        break;
    case 16:
        if (self->IsSigned)
        {
            const auto val = *reinterpret_cast<int16_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%hi", val);
        }
        else
        {
            const auto val = *reinterpret_cast<uint16_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%hu", val);
        }
        ptr += sizeof(int16_t);
        break;
    case 32:
        if (self->IsSigned)
        {
            const auto val = *reinterpret_cast<int32_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%li", val);
        }
        else
        {
            const auto val = *reinterpret_cast<uint32_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%lu", val);
        }
        ptr += sizeof(int32_t);
        break;
    case 64:
        if (self->IsSigned)
        {
            const auto val = *reinterpret_cast<int64_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%lli", val);
        }
        else
        {
            const auto val = *reinterpret_cast<uint64_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%llu", val);
        }
        ptr += sizeof(int64_t);
        break;
    default: break;
    }
}

void FP_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = static_cast<FPType*>(type);
    const auto val = va_arg(ap, double);
    offset += snprintf(stream + offset, n - offset, "%f", val);

    (void)self;
}

void FP_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = static_cast<FPType*>(type);

    switch (self->Bits)
    {
    case 32:
        {
            const auto val = *reinterpret_cast<float*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%f", val);
        }
        ptr += sizeof(float);
        break;
    case 64:
        {
            const auto val = *reinterpret_cast<double*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%f", val);
        }
        ptr += sizeof(double);
        break;
    default: break;
    }

    (void)self;
}

void Pointer_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = static_cast<PointerType*>(type);
    const auto ptr = va_arg(ap, char*);
    offset += snprintf(stream + offset, n - offset, "%p", ptr);

    (void)self;
}

void Pointer_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = static_cast<PointerType*>(type);
    offset += snprintf(stream + offset, n - offset, "%p", *reinterpret_cast<char**>(ptr));
    ptr += sizeof(char*);

    (void)self;
}

void Array_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = static_cast<ArrayType*>(type);
    auto ptr = va_arg(ap, char*);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->Element->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Array_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = static_cast<ArrayType*>(type);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->Element->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Tuple_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = static_cast<TupleType*>(type);
    auto ptr = va_arg(ap, char*);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->Elements[i]->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Tuple_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = static_cast<TupleType*>(type);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->Elements[i]->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Struct_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = static_cast<StructType*>(type);
    auto ptr = va_arg(ap, char*);

    offset += snprintf(stream + offset, n - offset, "{ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        offset += snprintf(stream + offset, n - offset, "%s: ", self->Elements[i].first);
        self->Elements[i].second->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " }");
}

void Struct_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = static_cast<StructType*>(type);

    offset += snprintf(stream + offset, n - offset, "{ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        offset += snprintf(stream + offset, n - offset, "%s: ", self->Elements[i].first);
        self->Elements[i].second->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " }");
}

void Function_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list&)
{
    const auto self = static_cast<FunctionType*>(type);
    offset += snprintf(stream + offset, n - offset, "fn");

    (void)self;
}

void Function_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*&)
{
    const auto self = static_cast<FunctionType*>(type);
    offset += snprintf(stream + offset, n - offset, "fn");

    (void)self;
}

void format(char* stream, const unsigned n, ...)
{
    va_list ap;
    va_start(ap, n);

    unsigned offset = 0;
    while (auto type = ParseType(ap))
    {
        type->Append(stream, n, offset, ap);
        delete type;
    }

    va_end(ap);
}
