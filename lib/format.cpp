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
struct PrimitiveType;
struct ArrayType;
struct TupleType;
struct ObjectType;
struct FunctionType;

static void Primitive_AppendV(Type*, char*, uint64_t, uint64_t&, va_list&);
static void Primitive_AppendP(Type*, char*, uint64_t, uint64_t&, char*&);
static void Array_AppendV(Type*, char*, uint64_t, uint64_t&, va_list&);
static void Array_AppendP(Type*, char*, uint64_t, uint64_t&, char*&);
static void Tuple_AppendV(Type*, char*, uint64_t, uint64_t&, va_list&);
static void Tuple_AppendP(Type*, char*, uint64_t, uint64_t&, char*&);
static void Object_AppendV(Type*, char*, uint64_t, uint64_t&, va_list&);
static void Object_AppendP(Type*, char*, uint64_t, uint64_t&, char*&);
static void Function_AppendV(Type*, char*, uint64_t, uint64_t&, va_list&);
static void Function_AppendP(Type*, char*, uint64_t, uint64_t&, char*&);

struct Type
{
    using AppendVProc = void(*)(Type*, char*, uint64_t, uint64_t&, va_list&);
    using AppendPProc = void(*)(Type*, char*, uint64_t, uint64_t&, char*&);

    Type(const AppendVProc append_v, const AppendPProc append_p)
        : AppendV(append_v), AppendP(append_p)
    {
    }

    void Append(char* stream, const uint64_t n, uint64_t& offset, va_list& ap)
    {
        AppendV(this, stream, n, offset, ap);
    }

    void AppendPtr(char* stream, const uint64_t n, uint64_t& offset, char*& ptr)
    {
        AppendP(this, stream, n, offset, ptr);
    }

    AppendVProc AppendV;
    AppendPProc AppendP;
};

struct PrimitiveType : Type
{
    explicit PrimitiveType(const int id)
        : Type(Primitive_AppendV, Primitive_AppendP), ID(id)
    {
    }

    int ID;
};

struct ArrayType : Type
{
    ArrayType(Type* element_type, const size_t element_count)
        : Type(Array_AppendV, Array_AppendP), ElementType(element_type), ElementCount(element_count)
    {
    }

    ~ArrayType()
    {
        delete ElementType;
    }

    Type* ElementType;
    size_t ElementCount;
};

struct TupleType : Type
{
    explicit TupleType(Type** element_types, const size_t element_count)
        : Type(Tuple_AppendV, Tuple_AppendP), ElementTypes(element_types), ElementCount(element_count)
    {
    }

    ~TupleType()
    {
        for (size_t i = 0; i < ElementCount; ++i)
            delete ElementTypes[i];
        delete[] ElementTypes;
    }

    Type** ElementTypes;
    size_t ElementCount;
};

struct ObjectType : Type
{
    explicit ObjectType(Pair<const char*, Type*>* element_types, const size_t element_count)
        : Type(Object_AppendV, Object_AppendP), ElementTypes(element_types), ElementCount(element_count)
    {
    }

    ~ObjectType()
    {
        for (size_t i = 0; i < ElementCount; ++i)
            delete ElementTypes[i].second;
        delete[] ElementTypes;
    }

    Pair<const char*, Type*>* ElementTypes;
    size_t ElementCount;
};

struct FunctionType : Type
{
    FunctionType(Type* result_type, Type** param_types, const size_t param_count, const bool vararg)
        : Type(Function_AppendV, Function_AppendP),
          ResultType(result_type),
          ParamTypes(param_types),
          ParamCount(param_count),
          VarArg(vararg)
    {
    }

    ~FunctionType()
    {
        delete ResultType;
        for (size_t i = 0; i < ParamCount; ++i)
            delete ParamTypes[i];
        delete[] ParamTypes;
    }

    Type* ResultType;
    Type** ParamTypes;
    size_t ParamCount;
    bool VarArg;
};

Type* ParseType(va_list& ap)
{
    switch (const auto id = va_arg(ap, int))
    {
    case ID_BOOLEAN:
    case ID_NUMBER:
    case ID_STRING:
        return new PrimitiveType(id);

    case ID_ARRAY:
        {
            const auto element_count = va_arg(ap, size_t);
            const auto element_type = ParseType(ap);
            return new ArrayType(element_type, element_count);
        }

    case ID_TUPLE:
        {
            const auto element_count = va_arg(ap, size_t);
            const auto element_types = New<Type*>(element_count);
            for (size_t i = 0; i < element_count; ++i)
                element_types[i] = ParseType(ap);
            return new TupleType(element_types, element_count);
        }

    case ID_OBJECT:
        {
            const auto element_count = va_arg(ap, size_t);
            const auto element_types = New<Pair<const char*, Type*>>(element_count);
            for (size_t i = 0; i < element_count; ++i)
            {
                const auto name = va_arg(ap, const char*);
                const auto type = ParseType(ap);
                element_types[i] = {name, type};
            }
            return new ObjectType(element_types, element_count);
        }

    case ID_FUNCTION:
        {
            const auto result_type = ParseType(ap);
            const auto param_count = va_arg(ap, size_t);
            const auto param_types = New<Type*>(param_count);
            for (size_t i = 0; i < param_count; ++i)
                param_types[i] = ParseType(ap);
            const auto vararg = va_arg(ap, int);
            return new FunctionType(result_type, param_types, param_count, vararg);
        }

    default:
    case ID_VOID:
        return {};
    }
}

void Primitive_AppendV(Type* type, char* stream, const uint64_t n, uint64_t& offset, va_list& ap)
{
    const auto self = static_cast<PrimitiveType*>(type);
    switch (self->ID)
    {
    case ID_BOOLEAN:
        offset += snprintf(stream + offset, n - offset, "%s", va_arg(ap, int) ? "true" : "false");
        break;
    case ID_NUMBER:
        {
            const auto val = va_arg(ap, double);
            offset += snprintf(stream + offset, n - offset, val == round(val) ? "%.0f" : "%f", val);
        }
        break;
    case ID_STRING:
        offset += snprintf(stream + offset, n - offset, "%s", va_arg(ap, const char*));
        break;
    default: break;
    }
}

void Primitive_AppendP(Type* type, char* stream, const uint64_t n, uint64_t& offset, char*& ptr)
{
    const auto self = static_cast<PrimitiveType*>(type);
    switch (self->ID)
    {
    case ID_BOOLEAN:
        offset += snprintf(stream + offset, n - offset, "%s", *reinterpret_cast<int*>(ptr) ? "true" : "false");
        ptr += sizeof(int);
        break;
    case ID_NUMBER:
        {
            const auto val = *reinterpret_cast<double*>(ptr);
            offset += snprintf(stream + offset, n - offset, val == round(val) ? "%.0f" : "%f", val);
        }
        ptr += sizeof(double);
        break;
    case ID_STRING:
        offset += snprintf(stream + offset, n - offset, "%s", *reinterpret_cast<const char**>(ptr));
        ptr += sizeof(const char*);
        break;
    default: break;
    }
}

void Array_AppendV(Type* type, char* stream, const uint64_t n, uint64_t& offset, va_list& ap)
{
    const auto self = static_cast<ArrayType*>(type);
    auto ptr = va_arg(ap, char*);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (size_t i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->ElementType->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Array_AppendP(Type* type, char* stream, const uint64_t n, uint64_t& offset, char*& ptr)
{
    const auto self = static_cast<ArrayType*>(type);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (size_t i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->ElementType->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Tuple_AppendV(Type* type, char* stream, const uint64_t n, uint64_t& offset, va_list& ap)
{
    const auto self = static_cast<TupleType*>(type);
    auto ptr = va_arg(ap, char*);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (size_t i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->ElementTypes[i]->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Tuple_AppendP(Type* type, char* stream, const uint64_t n, uint64_t& offset, char*& ptr)
{
    const auto self = static_cast<TupleType*>(type);

    offset += snprintf(stream + offset, n - offset, "[ ");
    for (size_t i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        self->ElementTypes[i]->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " ]");
}

void Object_AppendV(Type* type, char* stream, const uint64_t n, uint64_t& offset, va_list& ap)
{
    const auto self = static_cast<ObjectType*>(type);
    auto ptr = va_arg(ap, char*);

    offset += snprintf(stream + offset, n - offset, "{ ");
    for (size_t i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        offset += snprintf(stream + offset, n - offset, "%s: ", self->ElementTypes[i].first);
        self->ElementTypes[i].second->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " }");
}

void Object_AppendP(Type* type, char* stream, const uint64_t n, uint64_t& offset, char*& ptr)
{
    const auto self = static_cast<ObjectType*>(type);

    offset += snprintf(stream + offset, n - offset, "{ ");
    for (size_t i = 0; i < self->ElementCount; ++i)
    {
        if (i > 0) offset += snprintf(stream + offset, n - offset, ", ");
        offset += snprintf(stream + offset, n - offset, "%s: ", self->ElementTypes[i].first);
        self->ElementTypes[i].second->AppendPtr(stream, n, offset, ptr);
    }
    offset += snprintf(stream + offset, n - offset, " }");
}

void Function_AppendV(Type* type, char* stream, const uint64_t n, uint64_t& offset, va_list& ap)
{
    const auto self = static_cast<FunctionType*>(type);
    const auto ptr = va_arg(ap, char*);
    offset += snprintf(stream + offset, n - offset, "%p", ptr);
}

void Function_AppendP(Type* type, char* stream, const uint64_t n, uint64_t& offset, char*& ptr)
{
    const auto self = static_cast<FunctionType*>(type);
    offset += snprintf(stream + offset, n - offset, "%p", ptr);
    ptr += sizeof(char*);
}

void format(char* stream, const uint64_t n, ...)
{
    va_list ap;
    va_start(ap, n);

    uint64_t offset = 0;
    while (auto type = ParseType(ap))
    {
        type->Append(stream, n, offset, ap);
        delete type;
    }

    va_end(ap);
}
