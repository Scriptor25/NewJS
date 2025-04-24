#include <cinttypes>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <newjs/std.hpp>

template<typename T>
class allocator
{
public:
    static T *malloc(const size_t count)
    {
        // fprintf(stderr, "malloc(%llu)\n", count * sizeof(T));
        return static_cast<T *>(::malloc(count * sizeof(T)));
    }

    static T *calloc(const size_t count)
    {
        // fprintf(stderr, "calloc(%llu, %llu)\n", count, sizeof(T));
        return static_cast<T *>(::calloc(count, sizeof(T)));
    }

    static T *realloc(T *memory, const size_t new_count)
    {
        // fprintf(stderr, "realloc(%p, %llu)\n", memory, new_count * sizeof(T));
        return static_cast<T *>(::realloc(memory, new_count * sizeof(T)));
    }

    static void free(T *memory)
    {
        // fprintf(stderr, "free(%p)\n", memory);
        ::free(memory);
    }

    allocator() = delete;
    ~allocator() = delete;
};

void *operator new(const size_t size)
{
    return malloc(size);
}

void *operator new[](const size_t size)
{
    return malloc(size);
}

void operator delete(void *block) noexcept
{
    free(block);
}

void operator delete(void *block, size_t) noexcept
{
    free(block);
}

void operator delete[](void *block) noexcept
{
    free(block);
}

void operator delete[](void *block, size_t) noexcept
{
    free(block);
}

class string
{
public:
    string()
    {
        length = 0;
        reserved = 0;
        data = nullptr;
    }

    string(const string &) = delete;
    string &operator=(const string &) = delete;

    ~string()
    {
        allocator<char>::free(data);
    }

    template<typename... Args>
    void append(const char *format, Args &&... args)
    {
        constexpr auto BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        auto buffer_length = snprintf(buffer, BUFFER_SIZE, format, std::forward<Args>(args)...);
        buffer[buffer_length] = 0;
        append(buffer);
    }

    void append(const char *s)
    {
        const auto str_len = strlen(s);
        reserve(str_len);
        memcpy(data + length, s, str_len);
        length += str_len;
    }

    void reserve(const unsigned count)
    {
        if (!count)
            return;
        const auto new_length = length + count;
        if (reserved >= new_length)
            return;
        auto new_reserved = reserved ? reserved : 1;
        while (new_reserved < new_length)
            new_reserved <<= 1;
        reserved = new_reserved;
        data = allocator<char>::realloc(data, reserved);
    }

    [[nodiscard]] const char *copy() const
    {
        if (!length)
            return nullptr;
        const auto data_copy = allocator<char>::malloc(length + 1);
        memcpy(data_copy, data, length);
        data_copy[length] = 0;
        return data_copy;
    }

private:
    unsigned length;
    unsigned reserved;
    char *data;
};

template<typename F, typename S>
struct pair
{
    F first;
    S second;
};

struct Type;
struct IntegerType;
struct FloatingPointType;
struct PointerType;
struct ArrayType;
struct StructType;
struct TupleType;
struct FunctionType;

static void Incomplete_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void Incomplete_AppendP(Type *type, string &buffer, char *&ptr);
static void Integer_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void Integer_AppendP(Type *type, string &buffer, char *&ptr);
static void FloatingPoint_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void FloatingPoint_AppendP(Type *type, string &buffer, char *&ptr);
static void Pointer_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void Pointer_AppendP(Type *type, string &buffer, char *&ptr);
static void Array_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void Array_AppendP(Type *type, string &buffer, char *&ptr);
static void Struct_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void Struct_AppendP(Type *type, string &buffer, char *&ptr);
static void Tuple_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void Tuple_AppendP(Type *type, string &buffer, char *&ptr);
static void Function_AppendV(Type *type, string &buffer, va_list &arg_ptr);
static void Function_AppendP(Type *type, string &buffer, char *&ptr);

struct Type
{
    using AppendVProc = void(*)(Type *type, string &buffer, va_list &arg_ptr);
    using AppendPProc = void(*)(Type *type, string &buffer, char *&ptr);

    Type(const unsigned id, const AppendVProc append_v, const AppendPProc append_p)
        : ID(id),
          AppendV(append_v),
          AppendP(append_p)
    {
    }

    void Append(string &buffer, va_list &arg_ptr)
    {
        AppendV(this, buffer, arg_ptr);
    }

    void AppendPtr(string &buffer, char *&ptr)
    {
        AppendP(this, buffer, ptr);
    }

    unsigned ID;
    AppendVProc AppendV;
    AppendPProc AppendP;
};

struct IncompleteType final : Type
{
    explicit IncompleteType(const char *name)
        : Type(ID_INCOMPLETE, Incomplete_AppendV, Incomplete_AppendP),
          Name(name)
    {
    }

    const char *Name;
};

struct IntegerType final : Type
{
    IntegerType(const unsigned bits, const bool is_signed)
        : Type(ID_INTEGER, Integer_AppendV, Integer_AppendP),
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
        : Type(ID_FLOATING_POINT, FloatingPoint_AppendV, FloatingPoint_AppendP),
          Bits(bits)
    {
    }

    unsigned Bits;
};

struct PointerType final : Type
{
    explicit PointerType(Type *element_type)
        : Type(ID_POINTER, Pointer_AppendV, Pointer_AppendP),
          ElementType(element_type)
    {
    }

    Type *ElementType;
};

struct ArrayType final : Type
{
    ArrayType(Type *element_type, const unsigned element_count)
        : Type(ID_ARRAY, Array_AppendV, Array_AppendP),
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
    StructType(pair<const char *, Type *> *element_types, const unsigned element_count)
        : Type(ID_STRUCT, Struct_AppendV, Struct_AppendP),
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

    pair<const char *, Type *> *ElementTypes;
    unsigned ElementCount;
};

struct TupleType final : Type
{
    TupleType(Type **element_types, const unsigned element_count)
        : Type(ID_TUPLE, Tuple_AppendV, Tuple_AppendP),
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
        : Type(ID_FUNCTION, Function_AppendV, Function_AppendP)
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
            const auto elements = new pair<const char *, Type *>[element_count];
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
            const auto elements = new Type *[element_count];
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

void Incomplete_AppendV(Type *type, string &buffer, va_list &)
{
    const auto self = reinterpret_cast<IncompleteType *>(type);
    buffer.append("[%s]", self->Name);
}

void Incomplete_AppendP(Type *type, string &buffer, char *&)
{
    const auto self = reinterpret_cast<IncompleteType *>(type);
    buffer.append("[%s]", self->Name);
}

void Integer_AppendV(Type *type, string &buffer, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<IntegerType *>(type);
    switch (self->Bits)
    {
        case 1:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int);
                buffer.append(val ? "true" : "false");
            }
            else
            {
                const auto val = va_arg(arg_ptr, unsigned);
                buffer.append(val ? "true" : "false");
            }
            break;
        case 8:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int);
                buffer.append("%" PRId8, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, unsigned);
                buffer.append("%" PRIu8, val);
            }
            break;
        case 16:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int);
                buffer.append("%" PRId16, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, unsigned);
                buffer.append("%" PRIu16, val);
            }
            break;
        case 32:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int32_t);
                buffer.append("%" PRId32, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, uint32_t);
                buffer.append("%" PRIu32, val);
            }
            break;
        case 64:
            if (self->IsSigned)
            {
                const auto val = va_arg(arg_ptr, int64_t);
                buffer.append("%" PRId64, val);
            }
            else
            {
                const auto val = va_arg(arg_ptr, uint64_t);
                buffer.append("%" PRIu64, val);
            }
            break;
        default:
            break;
    }
}

void Integer_AppendP(Type *type, string &buffer, char *&ptr)
{
    const auto self = reinterpret_cast<IntegerType *>(type);
    switch (self->Bits)
    {
        case 1:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int8_t *>(ptr);
                buffer.append(val ? "true" : "false");
            }
            else
            {
                const auto val = *reinterpret_cast<uint8_t *>(ptr);
                buffer.append(val ? "true" : "false");
            }
            break;
        case 8:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int8_t *>(ptr);
                buffer.append("%" PRId8, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint8_t *>(ptr);
                buffer.append("%" PRIu8, val);
            }
            break;
        case 16:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int16_t *>(ptr);
                buffer.append("%" PRId16, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint16_t *>(ptr);
                buffer.append("%" PRIu16, val);
            }
            break;
        case 32:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int32_t *>(ptr);
                buffer.append("%" PRId32, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint32_t *>(ptr);
                buffer.append("%" PRIu32, val);
            }
            break;
        case 64:
            if (self->IsSigned)
            {
                const auto val = *reinterpret_cast<int64_t *>(ptr);
                buffer.append("%" PRId64, val);
            }
            else
            {
                const auto val = *reinterpret_cast<uint64_t *>(ptr);
                buffer.append("%" PRIu64, val);
            }
            break;
        default:
            break;
    }

    ptr += self->Bits / 8;
}

void FloatingPoint_AppendV(Type *type, string &buffer, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<FloatingPointType *>(type);

    const auto val = va_arg(arg_ptr, double);
    buffer.append("%f", val);

    (void) self;
}

void FloatingPoint_AppendP(Type *type, string &buffer, char *&ptr)
{
    const auto self = reinterpret_cast<FloatingPointType *>(type);

    switch (self->Bits)
    {
        case 32:
        {
            const auto val = *reinterpret_cast<float *>(ptr);
            buffer.append("%f", val);
        }
        break;
        case 64:
        {
            const auto val = *reinterpret_cast<double *>(ptr);
            buffer.append("%f", val);
        }
        break;
        default:
            break;
    }

    ptr += self->Bits / 8;
}

void Pointer_AppendV(Type *type, string &buffer, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<PointerType *>(type);
    const auto ptr = va_arg(arg_ptr, char*);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType);
        self->ElementType->ID == ID_INTEGER && el->Bits == 8 && el->IsSigned)
    {
        buffer.append("%s", ptr);
        return;
    }

    buffer.append("0x%p", ptr);
}

void Pointer_AppendP(Type *type, string &buffer, char *&ptr)
{
    const auto self = reinterpret_cast<PointerType *>(type);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType);
        self->ElementType->ID == ID_INTEGER && el->Bits == 8 && el->IsSigned)
    {
        buffer.append("%s", *reinterpret_cast<char **>(ptr));
        ptr += sizeof(char *);
        return;
    }

    buffer.append("0x%p", *reinterpret_cast<char **>(ptr));
    ptr += sizeof(char *);
}

void Array_AppendV(Type *type, string &buffer, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<ArrayType *>(type);
    auto ptr = va_arg(arg_ptr, char*);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType);
        self->ElementType->ID == ID_INTEGER && el->Bits == 8 && el->IsSigned)
    {
        buffer.append("%.*s", self->ElementCount, ptr);
        return;
    }

    buffer.append("[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            buffer.append(", ");
        self->ElementType->AppendPtr(buffer, ptr);
    }
    buffer.append(" ]");
}

void Array_AppendP(Type *type, string &buffer, char *&ptr)
{
    const auto self = reinterpret_cast<ArrayType *>(type);

    if (const auto el = reinterpret_cast<IntegerType *>(self->ElementType);
        self->ElementType->ID == ID_INTEGER && el->Bits == 8 && el->IsSigned)
    {
        buffer.append("%.*s", self->ElementCount, ptr);
        ptr += self->ElementCount;
        return;
    }

    buffer.append("[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            buffer.append(", ");
        self->ElementType->AppendPtr(buffer, ptr);
    }
    buffer.append(" ]");
}

void Struct_AppendV(Type *type, string &buffer, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<StructType *>(type);
    auto ptr = va_arg(arg_ptr, char*);

    buffer.append("{ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            buffer.append(", ");
        buffer.append("%s: ", self->ElementTypes[i].first);
        self->ElementTypes[i].second->AppendPtr(buffer, ptr);
    }
    buffer.append(" }");
}

void Struct_AppendP(Type *type, string &buffer, char *&ptr)
{
    const auto self = reinterpret_cast<StructType *>(type);

    buffer.append("{ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            buffer.append(", ");
        buffer.append("%s: ", self->ElementTypes[i].first);
        self->ElementTypes[i].second->AppendPtr(buffer, ptr);
    }
    buffer.append(" }");
}

void Tuple_AppendV(Type *type, string &buffer, va_list &arg_ptr)
{
    const auto self = reinterpret_cast<TupleType *>(type);
    auto ptr = va_arg(arg_ptr, char*);

    buffer.append("[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            buffer.append(", ");
        self->ElementTypes[i]->AppendPtr(buffer, ptr);
    }
    buffer.append(" ]");
}

void Tuple_AppendP(Type *type, string &buffer, char *&ptr)
{
    const auto self = reinterpret_cast<TupleType *>(type);

    buffer.append("[ ");
    for (unsigned i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0)
            buffer.append(", ");
        self->ElementTypes[i]->AppendPtr(buffer, ptr);
    }
    buffer.append(" ]");
}

void Function_AppendV(Type *, string &buffer, va_list &arg_ptr)
{
    const auto ptr = va_arg(arg_ptr, char*);
    buffer.append("fn[%p]", ptr);
}

void Function_AppendP(Type *, string &buffer, char *&ptr)
{
    buffer.append("fn[%p]", *reinterpret_cast<char **>(ptr));
    ptr += sizeof(char *);
}

const char *format(const unsigned count, ...)
{
    string buffer;

    va_list arg_ptr;
    va_start(arg_ptr, count);

    for (unsigned i = 0; i < count; ++i)
    {
        const auto type = ParseType(arg_ptr);
        if (!type)
            break;
        type->Append(buffer, arg_ptr);
        delete type;
    }

    va_end(arg_ptr);
    return buffer.copy();
}
