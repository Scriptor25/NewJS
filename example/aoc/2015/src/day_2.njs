import { part_err } from "./main.njs"

extern parse_int(str: i8[]): u64
extern println(str: i8[])
extern malloc(bytes: u64): void[]
extern realloc(block: void[], bytes: u64): void[]

function min(a: u64, b: u64): u64 {
    return a < b ? a : b
}

function parse_input(input: i8[]): [u64[3][], u64] {
    let sizes = malloc(24u64) as u64[3][]
    let len = 1u64

    let num: i8[512]
    let num_len: u64

    let i: u64
    for (const c of input) {
        if (c == 'x' || c == '\n') {
            num[num_len] = '\x00'
            sizes[len - 1u64][i] = parse_int(num)
            num_len = 0u64
            i = (i + 1u64) % 3u64
            if (c == '\n') {
                ++len
                sizes = realloc(sizes, len * 24u64)
            }
        } else {
            num[num_len++] = c
        }
    }

    return [sizes, len]
}

function part_1(input: i8[]): u64 {
    const [sizes, len] = parse_input(input)

    println($"{sizes}")

    let sum = 0u64
    for (let i: u64; i < len; ++i) {
        const size = sizes[i]
        const area1 = size[0u64] * size[1u64]
        const area2 = size[1u64] * size[2u64]
        const area3 = size[2u64] * size[0u64]
        const min = min(area1, min(area2, area3))
        sum += area1 * 2u64 + area2 * 2u64 + area3 * 2u64 + min

        println($"{area1} {area2} {area3}")
    }

    return sum
}

function part_2(input: i8[]): u64 {
    return 0u64
}

function day(part: u64): (i8[]): u64 {
    return part == 1u64 ? part_1 : part == 2u64 ? part_2 : part_err
}
