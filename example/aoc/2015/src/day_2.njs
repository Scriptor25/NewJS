import { part_err } from "./main.njs"

extern parse_int(str: string): number
extern println(str: string)

function min(a: number, b: number): number {
    return a < b ? a : b
}

function parse_input(input: string): number[3][] {
    let sizes: number[3][]
    sizes << [0, 0, 0]

    let i = 0
    let num: char[]
    for (const c of input) {
        if (c == 'x' || c == '\n') {
            sizes[sizes.length - 1][i] = parse_int(num)
            !num
            i = (i + 1) % 3
            if (c == '\n')
                sizes << [0, 0, 0]
        } else {
            num << c
        }
    }

    sizes >> _
    return sizes
}

function part_1(input: string): number {
    const sizes = parse_input(input)

    println($"{sizes}")

    let sum = 0
    for (const size of sizes) {
        const area1 = size[0] * size[1]
        const area2 = size[1] * size[2]
        const area3 = size[2] * size[0]
        const min = min(area1, min(area2, area3))
        sum += area1 * 2 + area2 * 2 + area3 * 2 + min

        println($"{area1} {area2} {area3}")
    }

    return sum
}

function part_2(input: string): number {
    return 0
}

function day(part: number): (string): number {
    return part == 1 ? part_1 : part == 2 ? part_2 : part_err
}
