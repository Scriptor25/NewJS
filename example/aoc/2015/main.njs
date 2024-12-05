import { day_01 } from "./day_01/day_01.njs"

extern println(message: string)

function day_err(input: string): number {
    println("invalid day")
    return 0
}

function part_err(input: string): number {
    println("invalid part")
    return 0
}

const day = 1
const part = 1
const input = ""

const fn = switch (day) {
    case 1  -> day_01(part)
    default -> day_err
}

const result = fn(input)
println($"result: {result}")
