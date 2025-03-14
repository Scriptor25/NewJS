type FILE = {}.FILE

extern function fopen(filename: i8[], mode: i8[]): FILE[]
extern function fclose(stream: FILE[]): i32
extern function fprintf(stream: FILE[], format: i8[], ...): i32
extern function fseek(stream: FILE[], offset: i64, origin: i32): i32
extern function fflush(stream: FILE[]): i32
extern function malloc(count: u64): void[]
extern function free(block: void[]): void

class image {
    open(&self: image, filename: i8[], width: u32, height: u32) {
        self.stream = fopen(filename, "wb")
        self.buffer = malloc(width * height * 3)
        self.width = width
        self.height = height
    },

    put(&self: image, x1: u32, x2: u32, r: i32, g: i32, b: i32) {
        self.buffer[(x1 + x2 * self.width) * 3 + 0] = r
        self.buffer[(x1 + x2 * self.width) * 3 + 1] = g
        self.buffer[(x1 + x2 * self.width) * 3 + 2] = b
    },

    flush(&self: image) {
        fseek(self.stream, 0, 0)
        fprintf(self.stream, "P6 %d %d 255 ", self.width, self.height)
        for (let i: u64; i < self.width * self.height * 3; ++i)
            fprintf(self.stream, "%c", self.buffer[i])
        fflush(self.stream)
    },

    close(&self: image) {
        fclose(self.stream)
        free(self.buffer)
        self.stream = 0
        self.buffer = 0
        self.width = 0
        self.height = 0
    },

    stream: FILE[],
    buffer: u8[],
    width: u32,
    height: u32,
}
