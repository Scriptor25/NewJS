extern fopen(filename: i8[], mode: i8[]): FILE[]
extern fclose(stream: FILE[])
extern fprintf(stream: FILE[], format: i8[], ...)

function begin(filename: i8[], width: u32, height: u32): FILE[] {
    const stream = fopen(filename, "wb")
    fprintf(stream, "P6 %d %d 255 ", width, height)
    return stream
}

function end(stream: FILE[]) {
    fclose(stream)
    stream = 0
}

function write(stream: FILE[], r: i32, g: i32, b: i32) {
    fprintf(stream, "%c%c%c", r, g, b)
}
