#include <cstdlib>
#include <NJS/Std.hpp>

double parse_int(const char* str)
{
    return static_cast<double>(atoll(str));
}

double parse_float(const char* str)
{
    return atof(str);
}
