#include <cstdlib>
#include <NJS/Std.hpp>

FILE* file_open(const char* filename, const char* mode)
{
    return fopen(filename, mode);
}

void file_close(FILE* stream)
{
    fclose(stream);
}

char* file_read(FILE* stream, uint64_t count)
{
    if (count <= 0) count = 1;

    auto buf = malloc(count + 1);

    const auto buf_size = fread(buf, 1, count, stream);
    static_cast<char*>(buf)[buf_size] = 0;
    if (const auto new_buf = realloc(buf, buf_size + 1))
        buf = new_buf;

    return static_cast<char*>(buf);
}

char* file_read_eof(FILE* stream)
{
    fseek(stream, 0, SEEK_END);
    auto count = ftell(stream);
    if (count <= 0) count = 1;
    fseek(stream, 0, SEEK_SET);

    auto buf = malloc(count + 1);

    const auto buf_size = fread(buf, 1, count, stream);
    static_cast<char*>(buf)[buf_size] = 0;
    if (const auto new_buf = realloc(buf, buf_size + 1))
        buf = new_buf;

    return static_cast<char*>(buf);
}

void file_read_free(char* buf)
{
    free(buf);
}

void file_write(FILE* stream, const char* buf, const uint64_t count)
{
    auto int_count = static_cast<long>(count);
    if (int_count <= 0) int_count = 1;
    fwrite(buf, 1, int_count, stream);
}
