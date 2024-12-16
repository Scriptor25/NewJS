import { part_err } from "./main.njs"

function part_1(input: i8[]): u64 {
    let level = 0i64
    for (const c of input)
        if (c == '(') ++level
        else if (c == ')') --level
    return level as u64
}

function part_2(input: i8[]): u64 {
    let level = 0i64
    let idx = 1u64
    for (const c of input) {
        if (c == '(') ++level
        else if (c == ')') --level

        if (level == -1i64)
            return idx
        idx++
    }
    return ~0u64
}

function day(part: u64): (i8[]): u64 {
    return part == 1u64 ? part_1 : part == 2u64 ? part_2 : part_err
}
