#pragma once

#include <cstdint>
#include <cstdio>

enum TypeID
{
    ID_VOID,
    ID_INCOMPLETE,
    ID_INTEGER,
    ID_FLOATING_POINT,
    ID_POINTER,
    ID_ARRAY,
    ID_STRUCT,
    ID_TUPLE,
    ID_FUNCTION,
};

extern "C"
{
extern FILE *std_in;
extern FILE *std_out;
extern FILE *std_err;

extern int rand_max;

const char *format(unsigned count, ...);
void println(const char *message);
int64_t parse_int(const char *str);
double parse_float(const char *str);
}
