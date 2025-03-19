type FILE = {}.FILE

extern function fopen(filename: i8[], mode: i8[]): FILE[]
extern function fclose(stream: FILE[]): i32
extern function fprintf(stream: FILE[], format: i8[], ...): i32
extern function fseek(stream: FILE[], offset: i64, origin: i32): i32
extern function fflush(stream: FILE[]): i32
extern function malloc(count: u64): void[]
extern function free(block: void[]): void

class image {
    open(&{*}: image, f: i8[], w: u32, h: u32) {
        stream = fopen(f, "wb")
        buffer = malloc(w * h * 3)
        width = w
        height = h
    },

    put(&{ buffer, width }: image, x1: u32, x2: u32, r: i32, g: i32, b: i32) {
        buffer[(x1 + x2 * width) * 3 + 0] = r
        buffer[(x1 + x2 * width) * 3 + 1] = g
        buffer[(x1 + x2 * width) * 3 + 2] = b
    },

    flush(&{*}: image) {
        fseek(stream, 0, 0)
        fprintf(stream, "P6 %d %d 255 ", width, height)
        for (let i: u64; i < width * height * 3; ++i)
            fprintf(stream, "%c", buffer[i])
        fflush(stream)
    },

    close(&{*}: image) {
        fclose(stream)
        free(buffer)
        stream = 0
        buffer = 0
        width = 0
        height = 0
    },

    stream: FILE[],
    buffer: u8[],
    width: u32,
    height: u32,
}
