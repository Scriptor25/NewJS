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
    ID_FUNCTION,
};

extern "C" {
void format(char*, unsigned, ...);

void println(const char*);

FILE* file_open(const char*, const char*);
void file_close(FILE*);
char* file_read(FILE*, uint64_t);
char* file_read_eof(FILE*);
void file_read_free(char*);
void file_write(FILE*, const char*, uint64_t);

int64_t parse_int(const char*);
double parse_float(const char*);
}
