#include <cstdint>
#include <stdarg.h>

extern "C"
void format(char* dest, const int64_t num_static, const int64_t num_dynamic, ...)
{
    va_list ap;
    va_start(ap, num_dynamic);

    // ...

    va_end(ap);
}
