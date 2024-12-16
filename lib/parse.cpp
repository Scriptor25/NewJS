#include <cstdlib>
#include <NJS/Std.hpp>

uint64_t parse_int(const char* str)
{
    return static_cast<uint64_t>(atoll(str));
}

double parse_float(const char* str)
{
    return atof(str);
}
