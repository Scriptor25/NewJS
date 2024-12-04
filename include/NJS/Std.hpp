#pragma once

#include <cstdint>

#define ID_VOID 0
#define ID_BOOLEAN 1
#define ID_NUMBER 2
#define ID_STRING 3
#define ID_ARRAY 4
#define ID_TUPLE 5
#define ID_OBJECT 6
#define ID_FUNCTION 7

extern "C" {
void format(char*, uint64_t, ...);
void println(const char*);
}
