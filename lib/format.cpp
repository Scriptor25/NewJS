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
    auto ptr = malloc(count * sizeof(T));
    return static_cast<T*>(ptr);
}

template <typename A, typename B>
struct Pair
{
    A first;
    B second;
};

struct Type;
struct IntType;
struct FPType;
struct PointerType;
struct ArrayType;
struct StructType;
struct TupleType;
struct FunctionType;

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

Type* ParseType(va_list& ap)
{
    switch (va_arg(ap, unsigned))
    {
    case ID_VOID:
        return nullptr;

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

    default:
        return nullptr;
    }
}

void Int_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = reinterpret_cast<IntType*>(type);
    const auto val = va_arg(ap, int);

    if (self->Bits == 1)
    {
        offset += snprintf(stream + offset, n - offset, val ? "true" : "false");
        return;
    }

    offset += snprintf(stream + offset, n - offset, self->IsSigned ? "%i" : "%u", val);
}

void Int_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = reinterpret_cast<IntType*>(type);
    switch (self->Bits)
    {
    case 1:
        if (self->IsSigned)
        {
            const auto val = *reinterpret_cast<int8_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, val ? "true" : "false");
        }
        else
        {
            const auto val = *reinterpret_cast<uint8_t*>(ptr);
            offset += snprintf(stream + offset, n - offset, val ? "true" : "false");
        }
        break;
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
        break;
    default: break;
    }

    ptr += self->Bits / 8;
}

void FP_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = reinterpret_cast<FPType*>(type);

    const auto val = va_arg(ap, double);
    offset += snprintf(stream + offset, n - offset, "%f", val);

    (void)self;
}

void FP_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = reinterpret_cast<FPType*>(type);

    switch (self->Bits)
    {
    case 32:
        {
            const auto val = *reinterpret_cast<float*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%f", val);
        }
        break;
    case 64:
        {
            const auto val = *reinterpret_cast<double*>(ptr);
            offset += snprintf(stream + offset, n - offset, "%f", val);
        }
        break;
    default: break;
    }

    ptr += self->Bits / 8;
}

void Pointer_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = reinterpret_cast<PointerType*>(type);
    const auto ptr = va_arg(ap, char*);

    if (const auto el = reinterpret_cast<IntType*>(self->Element); el && el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(stream + offset, n - offset, "%s", ptr);
        return;
    }

    offset += snprintf(stream + offset, n - offset, "0x%p", ptr);
}

void Pointer_AppendP(Type* type, char* stream, const unsigned n, unsigned& offset, char*& ptr)
{
    const auto self = reinterpret_cast<PointerType*>(type);

    if (const auto el = reinterpret_cast<IntType*>(self->Element); el && el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(stream + offset, n - offset, "%s", *reinterpret_cast<char**>(ptr));
        ptr += sizeof(char*);
        return;
    }

    offset += snprintf(stream + offset, n - offset, "0x%p", *reinterpret_cast<char**>(ptr));
    ptr += sizeof(char*);
}

void Array_AppendV(Type* type, char* stream, const unsigned n, unsigned& offset, va_list& ap)
{
    const auto self = reinterpret_cast<ArrayType*>(type);
    auto ptr = va_arg(ap, char*);

    if (const auto el = reinterpret_cast<IntType*>(self->Element); el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(stream + offset, n - offset, "%.*s", self->ElementCount, ptr);
        return;
    }

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
    const auto self = reinterpret_cast<ArrayType*>(type);

    if (const auto el = reinterpret_cast<IntType*>(self->Element); el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(stream + offset, n - offset, "%.*s", self->ElementCount, ptr);
        ptr += self->ElementCount;
        return;
    }

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
    const auto self = reinterpret_cast<TupleType*>(type);
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
    const auto self = reinterpret_cast<TupleType*>(type);

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
    const auto self = reinterpret_cast<StructType*>(type);
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
    const auto self = reinterpret_cast<StructType*>(type);

    offset += snprintf(stream + offset, n - offset, "{ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        offset += snprintf(stream + offset, n - offset, "%s: ", self->Elements[i].first);
        self->Elements[i].second->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " }");
}

void format(char* stream, const unsigned n, ...)
{
    va_list ap;
    va_start(ap, n);

    unsigned offset = 0;
    while (const auto type = ParseType(ap))
    {
        type->Append(stream, n, offset, ap);
        delete type;
    }

    va_end(ap);
}
