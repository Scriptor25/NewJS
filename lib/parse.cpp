#include <cstdlib>
#include <NJS/Std.hpp>

int64_t parse_int(const char *str)
{
    return strtoll(str, nullptr, 10);
}

double parse_float(const char *str)
{
    return strtod(str, nullptr);
}
