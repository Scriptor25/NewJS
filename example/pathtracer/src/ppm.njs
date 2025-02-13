extern function fopen(filename: i8[], mode: i8[]): FILE[]
extern function fclose(stream: FILE[])
extern function fprintf(stream: FILE[], format: i8[], ...)

type image = {
    stream: FILE[],
    width: u32,
    height: u32,
}

function begin(filename: i8[], width: u32, height: u32): image {
    const stream = fopen(filename, "wb")
    fprintf(stream, "P6 %d %d 255 ", width, height)
    return {
        stream,
        width,
        height,
    }
}

function end(image: image&) {
    fclose(image.stream)
    image.stream = 0
}

function write(image: image&, r: i32, g: i32, b: i32) {
    fprintf(image.stream, "%c%c%c", r, g, b)
}
