import { part_err } from "./main.njs"

extern strlen(str: i8[]): u64
extern println(str: i8[])

function part_1(input: i8[]): u64 {
    let level: i64
    let input_len = strlen(input)
    for (let i: u64; i < input_len; ++i) {
        let c = input[i]
        if (c == '(') ++level
        else if (c == ')') --level
    }
    return level
}

function part_2(input: i8[]): u64 {
    let level: i64
    let input_len = strlen(input)
    for (let i: u64; i < input_len; ++i) {
        let c = input[i]
        if (c == '(') ++level
        else if (c == ')') --level

        if (level == -1)
            return i + 1
    }
    return 0
}

function day(part: u64): (i8[]) => u64 {
    return part == 1 ? part_1 : part == 2 ? part_2 : part_err
}
