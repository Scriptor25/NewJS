import { day: day_1 } from "./day_1.njs"
import { day: day_2 } from "./day_2.njs"

// std
type File = i8[]
extern println(message: i8[])
extern parse_int(str: i8[]): u64
extern parse_float(str: i8[]): u64
extern file_open(filename: i8[], flags: i8[]): File
extern file_close(stream: File)
extern file_read(stream: File, count: u64): i8[]
extern file_read_eof(stream: File): i8[]
extern file_read_free(buf: i8[])
extern file_write(stream: File, buf: i8[], count: u64)

println($"{process}")

const day = parse_int(process.argv[1u64])
const part = parse_int(process.argv[2u64])

function part_err(input: i8[]): u64 {
    println("invalid part")
    return 0u64
}

function day_err(input: i8[]): u64 {
    println("invalid day")
    return 0u64
}

const fn = switch (day) {
    case 1u64  -> day_1(part)
    case 2u64  -> day_2(part)
    default -> day_err
}

const stream = file_open($"input/{day}.txt", "r")
const input = file_read_eof(stream)

const result = fn(input)
println($"result: {result}")

file_read_free(input)
