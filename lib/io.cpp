#include <cstdlib>
#include <NJS/Std.hpp>

FILE *std_in = stdin;
FILE *std_out = stdout;
FILE *std_err = stderr;

int rand_max = RAND_MAX;

void println(const char *message)
{
    printf("%s\n", message);
}
