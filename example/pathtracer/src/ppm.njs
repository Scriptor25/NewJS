type FILE

extern function fopen(filename: i8[], mode: i8[]): FILE[]
extern function fclose(stream: FILE[])
extern function fprintf(stream: FILE[], format: i8[], ...)

type image = {
    stream: FILE[],
    width: u32,
    height: u32,
}

export function begin(filename: i8[], width: u32, height: u32): image {
    const stream = fopen(filename, "wb")
    fprintf(stream, "P6 %d %d 255 ", width, height)
    return {
        stream,
        width,
        height,
    }
}

export function end(&self: image) {
    fclose(self.stream)
    self.stream = 0
}

export function write(const &self: image, r: i32, g: i32, b: i32) {
    fprintf(self.stream, "%c%c%c", r, g, b)
}
