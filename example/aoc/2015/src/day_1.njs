import { part_err } from "./main.njs"

function part_1(input: string): number {
    let level = 0
    for (const c of input)
        if (c == '(') ++level
        else if (c == ')') --level
    return level
}

function part_2(input: string): number {
    let level = 0
    let idx = 1
    for (const c of input) {
        if (c == '(') ++level
        else if (c == ')') --level

        if (level == -1)
            return idx
        idx++
    }
    return level
}

function day(part: number): (string): number {
    return part == 1 ? part_1 : part == 2 ? part_2 : part_err
}
