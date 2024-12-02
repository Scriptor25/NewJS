#include <cstdio>

extern "C"
void println(const char* message)
{
    printf("%s\n", message);
}
