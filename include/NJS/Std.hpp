#pragma once

#include <cstdint>
#include <cstdio>

#define ID_VOID 0
#define ID_BOOLEAN 1
#define ID_NUMBER 2
#define ID_STRING 3
#define ID_ARRAY 4
#define ID_TUPLE 5
#define ID_OBJECT 6
#define ID_FUNCTION 7
#define ID_VECTOR 8
#define ID_CHAR 9

extern "C" {
void format(char*, uint64_t, ...);

void println(const char*);

FILE* file_open(const char*, const char*);
void file_close(FILE*);
char* file_read(FILE*, double);
char* file_read_eof(FILE*);
void file_read_free(char*);
void file_write(FILE*, const char*, double);

double parse_int(const char*);
double parse_float(const char*);
}
