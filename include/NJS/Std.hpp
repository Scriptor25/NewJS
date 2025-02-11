#pragma once

#include <cstdint>
#include <cstdio>

enum TypeID
{
    ID_VOID,
    ID_INT,
    ID_FP,
    ID_POINTER,
    ID_ARRAY,
    ID_STRUCT,
    ID_TUPLE,
};

extern "C"
{
void format(char *stream, unsigned n, ...);

void println(const char *message);

FILE *file_open(const char *filename, const char *mode);
void file_close(FILE *stream);
char *file_read(FILE *stream, uint64_t count);
char *file_read_eof(FILE *stream);
void file_read_free(char *buf);
void file_write(FILE *stream, const char *buf, uint64_t count);

int64_t parse_int(const char *str);
double parse_float(const char *str);
}
