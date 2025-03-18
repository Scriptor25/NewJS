import { day: day_1 } from "./day_1.njs"
import { day: day_2 } from "./day_2.njs"

#SEEK_SET "0:u32"
#SEEK_CUR "1:u32"
#SEEK_END "2:u32"

extern function println(message: i8[const])
extern function parse_int(str: i8[const]): i64
extern function parse_float(str: i8[const]): u64

type FILE = {}.FILE

extern function fopen(filename: i8[const], mode: i8[const]): FILE[]
extern function fclose(stream: FILE[])
extern function fseek(stream: FILE[], offset: i32, origin: i32): i32
extern function ftell(stream: FILE[]): i32
extern function fread(buffer: void[], size: u32, count: u32, stream: FILE[]): u32

extern function malloc(count: u32): void[]
extern function free(block: void[])

if (process.argc != 3) {
    println(f"not enough arguments, require 3, got {process.argc}")
    return 1
}

const day = parse_int(process.argv[1])
const part = parse_int(process.argv[2])

export function part_err(input: i8[const]): u64 {
    println("invalid part")
    return 1
}

function day_err(input: i8[const]): u64 {
    println("invalid day")
    return 1
}

const fn = switch (day) {
    case 1  -> day_1(part)
    case 2  -> day_2(part)
    default -> day_err
}

if (fn == day_err || fn == part_err)
    return fn(0)

const stream = fopen(f"input/{day}.txt", "r")
fseek(stream, 0, SEEK_END)
const stream_size = ftell(stream)
fseek(stream, 0, SEEK_SET)
const input: i8[] = malloc(stream_size)
fread(input, 1, stream_size, stream)
fclose(stream)

const result = fn(input)
println(f"result: {result}")

free(input)
