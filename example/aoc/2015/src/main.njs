import { day: day_1 } from "./day_1.njs"

// std
type File = string
extern println(message: string)
extern parse_int(str: string): number
extern parse_float(str: string): number
extern file_open(filename: string, flags: string): File
extern file_close(stream: File)
extern file_read(stream: File, count: number): string
extern file_read_eof(stream: File): string
extern file_read_free(buf: string)
extern file_write(stream: File, buf: string, count: number)

println($"{process}")

const day = parse_int(process.args[1])
const part = parse_int(process.args[2])

function part_err(input: string): number {
    println($"invalid part")
    return 0
}

function day_err(input: string): number {
    println($"invalid day")
    return 0
}

const fn = switch (day) {
    case 1  -> day_1(part)
    default -> day_err
}

const stream = file_open($"input/{day}/input.txt", "r")
const input = file_read_eof(stream)

const result = fn(input)
println($"result: {result}")

file_read_free(input)
