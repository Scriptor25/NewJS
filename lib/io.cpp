#include <NJS/Std.hpp>

FILE *std_in = stdin;
FILE *std_out = stdout;
FILE *std_err = stderr;

void println(const char *message)
{
    printf("%s\n", message);
}
