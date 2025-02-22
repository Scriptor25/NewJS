#include <cinttypes>
#include <cmath>
#include <cstdarg>
#include <NJS/Std.hpp>

void *operator new(const size_t size) noexcept
{
    return malloc(size);
}

void operator delete(void *block) noexcept
{
    free(block);
}

void operator delete(void *ptr, size_t) noexcept
{
    free(ptr);
}

template<typename T>
static T *New(const size_t count)
{
    constexpr auto size = sizeof(T);
    auto ptr = malloc(count * size);
    return static_cast<T *>(ptr);
}

template<typename A, typename B>
struct Pair
{
    A first;
    B second;
};

struct Type;
struct IntegerType;
struct FloatingPointType;
struct PointerType;
struct ArrayType;
struct StructType;
struct TupleType;
struct FunctionType;

static void Incomplete_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void Incomplete_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);
static void Integer_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void Integer_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);
static void FloatingPoint_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void FloatingPoint_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);
static void Pointer_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void Pointer_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);
static void Array_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void Array_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);
static void Struct_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void Struct_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);
static void Tuple_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void Tuple_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);
static void Function_AppendV(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
static void Function_AppendP(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);

struct Type
{
    using AppendVProc = void(*)(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, va_list &arg_ptr);
    using AppendPProc = void(*)(Type *type, char *buffer, unsigned buffer_size, unsigned &offset, char *&ptr);

    Type(const AppendVProc append_v, const AppendPProc append_p)
        : AppendV(append_v),
          AppendP(append_p)
    {
    }

    void Append(char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
    {
        AppendV(this, buffer, buffer_size, offset, arg_ptr);
    }

    void AppendPtr(char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
    {
        AppendP(this, buffer, buffer_size, offset, ptr);
    }

    AppendVProc AppendV;
    AppendPProc AppendP;
};

struct IncompleteType final : Type
{
    explicit IncompleteType(const char *name)
        : Type(Incomplete_AppendV, Incomplete_AppendP),
          Name(name)
    {
    }

    const char *Name;
};

struct IntegerType final : Type
{
    IntegerType(const unsigned bits, const bool is_signed)
        : Type(Integer_AppendV, Integer_AppendP),
          Bits(bits),
          IsSigned(is_signed)
    {
    }

    unsigned Bits;
    bool IsSigned;
};

struct FloatingPointType final : Type
{
    explicit FloatingPointType(const unsigned bits)
        : Type(FloatingPoint_AppendV, FloatingPoint_AppendP),
          Bits(bits)
    {
    }

    unsigned Bits;
};

struct PointerType final : Type
{
    explicit PointerType(Type *element_type)
        : Type(Pointer_AppendV, Pointer_AppendP),
          ElementType(element_type)
    {
    }

    Type *ElementType;
};

struct ArrayType final : Type
{
    ArrayType(Type *element_type, const unsigned element_count)
        : Type(Array_AppendV, Array_AppendP),
          ElementType(element_type),
          ElementCount(element_count)
    {
    }

    ~ArrayType()
    {
        delete ElementType;
    }

    Type *ElementType;
    unsigned ElementCount;
};

struct StructType final : Type
{
    StructType(Pair<const char *, Type *> *element_types, const unsigned element_count)
        : Type(Struct_AppendV, Struct_AppendP),
          ElementTypes(element_types),
          ElementCount(element_count)
    {
    }

    ~StructType()
    {
        for (unsigned i = 0; i < ElementCount; ++i)
            delete ElementTypes[i].second;
        delete[] ElementTypes;
    }

    Pair<const char *, Type *> *ElementTypes;
    unsigned ElementCount;
};

struct TupleType final : Type
{
    TupleType(Type **element_types, const unsigned element_count)
        : Type(Tuple_AppendV, Tuple_AppendP),
          ElementTypes(element_types),
          ElementCount(element_count)
    {
    }

    ~TupleType()
    {
        for (unsigned i = 0; i < ElementCount; ++i)
            delete ElementTypes[i];
        delete[] ElementTypes;
    }

    Type **ElementTypes;
    unsigned ElementCount;
};

struct FunctionType final : Type
{
    FunctionType()
        : Type(Function_AppendV, Function_AppendP)
    {
    }
};

Type *ParseType(va_list &arg_ptr)
{
    switch (va_arg(arg_ptr, unsigned))
    {
        case ID_VOID:
            return nullptr;

        case ID_INCOMPLETE:
        {
            const auto name = va_arg(arg_ptr, const char *);
            return new IncompleteType(name);
        }

        case ID_INTEGER:
        {
            const auto bits = va_arg(arg_ptr, unsigned);
            const auto is_signed = va_arg(arg_ptr, unsigned);
            return new IntegerType(bits, is_signed);
        }

        case ID_FLOATING_POINT:
        {
            const auto bits = va_arg(arg_ptr, unsigned);
            return new FloatingPointType(bits);
        }

        case ID_POINTER:
        {
            const auto element = ParseType(arg_ptr);
            return new PointerType(element);
        }

        case ID_ARRAY:
        {
            const auto element_count = va_arg(arg_ptr, unsigned);
            const auto element = ParseType(arg_ptr);
            return new ArrayType(element, element_count);
        }

        case ID_STRUCT:
        {
            const auto element_count = va_arg(arg_ptr, unsigned);
            const auto elements = New<Pair<const char *, Type *>>(element_count);
            for (unsigned i = 0; i < element_count; ++i)
            {
                const auto name = va_arg(arg_ptr, const char*);
                const auto type = ParseType(arg_ptr);
                elements[i] = {name, type};
            }
            return new StructType(elements, element_count);
        }

        case ID_TUPLE:
        {
            const auto element_count = va_arg(arg_ptr, unsigned);
            const auto elements = New<Type *>(element_count);
            for (unsigned i = 0; i < element_count; ++i)
                elements[i] = ParseType(arg_ptr);
            return new TupleType(elements, element_count);
        }

        case ID_FUNCTION:
            return new FunctionType();

        default:
            return nullptr;
    }
}

void Incomplete_AppendV(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<IncompleteType *>(type);
    offset += snprintf(buffer + offset, buffer_size - offset, "[%s]", self->Name);
}

void Incomplete_AppendP(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    const auto self = reinterpret_cast<IncompleteType *>(type);
    offset += snprintf(buffer + offset, buffer_size - offset, "[%s]", self->Name);
}

void Integer_AppendV(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<IntegerType *>(type);
    switch (self->Bits)
    {
        case 1:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int);
                offset += snprintf(buffer + offset, buffer_size - offset, val ? "true" : "false");
            }
            else
            {
                const auto val = va_arg(arg_ptr, unsigned);
                offset += snprintf(buffer + offset, buffer_size - offset, val ? "true" : "false");
            }
            break;
        case 8:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId8, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, unsigned);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu8, val);
            }
            break;
        case 16:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId16, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, unsigned);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu16, val);
            }
            break;
        case 32:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int32_t);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId32, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, uint32_t);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu32, val);
            }
            break;
        case 64:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int64_t);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId64, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, uint64_t);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu64, val);
            }
            break;
        default:
            break;
    }
}

void Integer_AppendP(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    const auto self = reinterpret_cast<IntegerType *>(type);
    switch (self->Bits)
    {
        case 1:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int8_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, val ? "true" : "false");
            }
            else
            {
                const auto val = *reinterpret_cast<uint8_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, val ? "true" : "false");
            }
            break;
        case 8:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int8_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId8, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint8_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu8, val);
            }
            break;
        case 16:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int16_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId16, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint16_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu16, val);
            }
            break;
        case 32:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int32_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId32, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint32_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu32, val);
            }
            break;
        case 64:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int64_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRId64, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint64_t *>(ptr);
                offset += snprintf(buffer + offset, buffer_size - offset, "%" PRIu64, val);
            }
            break;
        default:
            break;
    }

    ptr += self->Bits / 8;
}

void FloatingPoint_AppendV(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<FloatingPointType *>(type);

    const auto val = va_arg(arg_ptr, double);
    offset += snprintf(buffer + offset, buffer_size - offset, "%f", val);

    (void) self;
}

void FloatingPoint_AppendP(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    const auto self = reinterpret_cast<FloatingPointType *>(type);

    switch (self->Bits)
    {
        case 32:
        {
            const auto val = *reinterpret_cast<float *>(ptr);
            offset += snprintf(buffer + offset, buffer_size - offset, "%f", val);
        }
        break;
        case 64:
        {
            const auto val = *reinterpret_cast<double *>(ptr);
            offset += snprintf(buffer + offset, buffer_size - offset, "%f", val);
        }
        break;
        default:
            break;
    }

    ptr += self->Bits / 8;
}

void Pointer_AppendV(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<PointerType *>(type);
    const auto ptr = va_arg(arg_ptr, char*);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType); el && el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(buffer + offset, buffer_size - offset, "%s", ptr);
        return;
    }

    offset += snprintf(buffer + offset, buffer_size - offset, "0x%p", ptr);
}

void Pointer_AppendP(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    const auto self = reinterpret_cast<PointerType *>(type);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType); el && el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(buffer + offset, buffer_size - offset, "%s", *reinterpret_cast<char **>(ptr));
        ptr += sizeof(char *);
        return;
    }

    offset += snprintf(buffer + offset, buffer_size - offset, "0x%p", *reinterpret_cast<char **>(ptr));
    ptr += sizeof(char *);
}

void Array_AppendV(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<ArrayType *>(type);
    auto ptr = va_arg(arg_ptr, char*);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType); el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(buffer + offset, buffer_size - offset, "%.*s", self->ElementCount, ptr);
        return;
    }

    offset += snprintf(buffer + offset, buffer_size - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            offset += snprintf(buffer + offset, buffer_size - offset, ", ");
        self->ElementType->AppendPtr(buffer, buffer_size, offset, ptr);
    }
    offset += snprintf(buffer + offset, buffer_size - offset, " ]");
}

void Array_AppendP(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    const auto self = reinterpret_cast<ArrayType *>(type);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType); el->Bits == 8 && el->IsSigned)
    {
        offset += snprintf(buffer + offset, buffer_size - offset, "%.*s", self->ElementCount, ptr);
        ptr += self->ElementCount;
        return;
    }

    offset += snprintf(buffer + offset, buffer_size - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            offset += snprintf(buffer + offset, buffer_size - offset, ", ");
        self->ElementType->AppendPtr(buffer, buffer_size, offset, ptr);
    }
    offset += snprintf(buffer + offset, buffer_size - offset, " ]");
}

void Struct_AppendV(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<StructType *>(type);
    auto ptr = va_arg(arg_ptr, char*);

    offset += snprintf(buffer + offset, buffer_size - offset, "{ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            offset += snprintf(buffer + offset, buffer_size - offset, ", ");
        offset += snprintf(buffer + offset, buffer_size - offset, "%s: ", self->ElementTypes[i].first);
        self->ElementTypes[i].second->AppendPtr(buffer, buffer_size, offset, ptr);
    }
    offset += snprintf(buffer + offset, buffer_size - offset, " }");
}

void Struct_AppendP(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    const auto self = reinterpret_cast<StructType *>(type);

    offset += snprintf(buffer + offset, buffer_size - offset, "{ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            offset += snprintf(buffer + offset, buffer_size - offset, ", ");
        offset += snprintf(buffer + offset, buffer_size - offset, "%s: ", self->ElementTypes[i].first);
        self->ElementTypes[i].second->AppendPtr(buffer, buffer_size, offset, ptr);
    }
    offset += snprintf(buffer + offset, buffer_size - offset, " }");
}

void Tuple_AppendV(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<TupleType *>(type);
    auto ptr = va_arg(arg_ptr, char*);

    offset += snprintf(buffer + offset, buffer_size - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            offset += snprintf(buffer + offset, buffer_size - offset, ", ");
        self->ElementTypes[i]->AppendPtr(buffer, buffer_size, offset, ptr);
    }
    offset += snprintf(buffer + offset, buffer_size - offset, " ]");
}

void Tuple_AppendP(Type *type, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    const auto self = reinterpret_cast<TupleType *>(type);

    offset += snprintf(buffer + offset, buffer_size - offset, "[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            offset += snprintf(buffer + offset, buffer_size - offset, ", ");
        self->ElementTypes[i]->AppendPtr(buffer, buffer_size, offset, ptr);
    }
    offset += snprintf(buffer + offset, buffer_size - offset, " ]");
}

void Function_AppendV(Type *, char *buffer, const unsigned buffer_size, unsigned &offset, va_list &arg_ptr)
{
    const auto ptr = va_arg(arg_ptr, char*);
    offset += snprintf(buffer + offset, buffer_size - offset, "fn[%p]", ptr);
}

void Function_AppendP(Type *, char *buffer, const unsigned buffer_size, unsigned &offset, char *&ptr)
{
    offset += snprintf(buffer + offset, buffer_size - offset, "fn[%p]", *reinterpret_cast<char **>(ptr));
    ptr += sizeof(char *);
}

void format(char *buffer, const unsigned buffer_size, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, buffer_size);

    unsigned offset = 0;
    while (const auto type = ParseType(arg_ptr))
    {
        type->Append(buffer, buffer_size, offset, arg_ptr);
        delete type;
    }

    va_end(arg_ptr);
}
