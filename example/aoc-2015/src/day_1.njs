import { part_err } from "./main.njs"

extern function strlen(str: i8[const]): u64
extern function println(str: i8[const])

function part_1(input: i8[const]): u64 {
    let level: i64
    const input_len = strlen(input)
    for (let i: u64; i < input_len; ++i) {
        const c = input[i]
        if (c == '(') ++level
        else if (c == ')') --level
    }
    return level
}

function part_2(input: i8[const]): u64 {
    let level: i64
    const input_len = strlen(input)
    for (let i: u64; i < input_len; ++i) {
        const c = input[i]
        if (c == '(') ++level
        else if (c == ')') --level

        if (level == -1)
            return i + 1
    }
    return 0
}

export function day(part: u64): (i8[const]) => u64 {
    return part == 1 ? part_1 : part == 2 ? part_2 : part_err
}
