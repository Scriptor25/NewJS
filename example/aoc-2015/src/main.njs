import { day: day_1 } from "./day_1.njs"
import { day: day_2 } from "./day_2.njs"

// std
type File = i8[]
extern println(message: i8[])
extern parse_int(str: i8[]): i64
extern parse_float(str: i8[]): u64
extern file_open(filename: i8[], flags: i8[]): File
extern file_close(stream: File)
extern file_read(stream: File, count: u64): i8[]
extern file_read_eof(stream: File): i8[]
extern file_read_free(buf: i8[])
extern file_write(stream: File, buf: i8[], count: u64)

println($"{process}")

let day = parse_int(process.argv[1])
let part = parse_int(process.argv[2])

function part_err(input: i8[]): u64 {
    println("invalid part")
    return 0
}

function day_err(input: i8[]): u64 {
    println("invalid day")
    return 0
}

let fn = switch (day) {
    case 1  -> day_1(part)
    case 2  -> day_2(part)
    default -> day_err
}

let stream = file_open($"input/{day}.txt", "r")
let input = file_read_eof(stream)

let result = fn(input)
println($"result: {result}")

file_read_free(input)
