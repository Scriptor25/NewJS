#include <NJS/Std.hpp>

auto std_in = stdin;
auto std_out = stdout;
auto std_err = stderr;

void println(const char *message)
{
    printf("%s\n", message);
}
