#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <NJS/TypeId.hpp>

extern "C"
void format(char* stream, const uint64_t n, ...)
{
    va_list ap;
    va_start(ap, n);

    uint64_t offset = 0;
    while (const auto type = va_arg(ap, int))
        if (type == 1)
            offset += snprintf(stream + offset, n - offset, "%s", va_arg(ap, const char*));
        else
            switch (va_arg(ap, int))
            {
            case NJS::TypeId_Void:
                offset += snprintf(stream + offset, n - offset, "void");
                break;
            case NJS::TypeId_Boolean:
                {
                    const auto val = va_arg(ap, int);
                    offset += snprintf(stream + offset, n - offset, "%s", val ? "true" : "false");
                }
                break;
            case NJS::TypeId_Number:
                {
                    const auto val = va_arg(ap, double);
                    offset += snprintf(stream + offset, n - offset, "%f", val);
                }
                break;
            case NJS::TypeId_String:
                {
                    const auto val = va_arg(ap, const char*);
                    offset += snprintf(stream + offset, n - offset, "%s", val);
                }
                break;

            case NJS::TypeId_Array:
            case NJS::TypeId_Tuple:
            case NJS::TypeId_Object:
            case NJS::TypeId_Function:
                {
                    const auto val = va_arg(ap, const void*);
                    offset += snprintf(stream + offset, n - offset, "%p", val);
                }
                break;

            default: break;
            }

    va_end(ap);
}
