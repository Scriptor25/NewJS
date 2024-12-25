import { part_err } from "./main.njs"

extern parse_int(str: i8[]): u64
extern println(str: i8[])
extern malloc(bytes: u64): void[]
extern realloc(block: void[], bytes: u64): void[]
extern strlen(str: i8[]): u64
extern free(block: void[])

function min(a: u64, b: u64): u64 {
    return a < b ? a : b
}

function parse_input(input: i8[]): [u64[3][], u64] {
    let sizes: u64[3][] = malloc(24)
    let sizes_len: u64 = 1

    let num: i8[512]
    let num_len: u64

    let input_len = strlen(input)
    let i: u64
    for (let k: u64; k < input_len; ++k) {
        let c = input[k]
        if (c == 'x' || c == '\n') {
            num[num_len] = '\x00'
            sizes[sizes_len - 1][i] = parse_int(&num[0])
            num_len = 0
            i = (i + 1) % 3
            if (c == '\n') {
                ++sizes_len
                sizes = realloc(sizes, sizes_len * 24)
            }
        } else {
            num[num_len++] = c
        }
    }

    return [sizes, sizes_len - 1:u64]
}

function swap(size: u64[3]&, a: u64, b: u64) {
    let t = size[a]
    size[a] = size[b]
    size[b] = t
}

function order(size: u64[3]&) {
    for (let i: u64; i < 2; ++i)
    for (let j: u64; j < 2; ++j)
    if (size[j] > size[j + 1]) {
        swap(size, j, j + 1)
    }
}

function part_1(input: i8[]): u64 {
    let [sizes, sizes_len] = parse_input(input)

    let sum: u64
    for (let i: u64; i < sizes_len; ++i) {
        let size = sizes[i]
        let area1 = size[0] * size[1]
        let area2 = size[1] * size[2]
        let area3 = size[2] * size[0]
        let min = min(area1, min(area2, area3))
        sum += area1 * 2 + area2 * 2 + area3 * 2 + min
    }

    free(sizes)
    return sum
}

function part_2(input: i8[]): u64 {
    let [sizes, sizes_len] = parse_input(input)

    let sum: u64
    for (let i: u64; i < sizes_len; ++i) {
        let size = sizes[i]
        order(size)
        sum += size[0] * 2 + size[1] * 2 + size[0] * size[1] * size[2]
    }

    free(sizes)
    return sum
}

function day(part: u64): (i8[]): u64 {
    return part == 1 ? part_1 : part == 2 ? part_2 : part_err
}
