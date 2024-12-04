function day_01_1(input: string): number
function day_01_2(input: string): number
function day_02_1(input: string): number
function day_02_2(input: string): number
function day_03_1(input: string): number
function day_03_2(input: string): number
function day_04_1(input: string): number
function day_04_2(input: string): number
function day_05_1(input: string): number
function day_05_2(input: string): number
function day_06_1(input: string): number
function day_06_2(input: string): number
function day_07_1(input: string): number
function day_07_2(input: string): number
function day_08_1(input: string): number
function day_08_2(input: string): number
function day_09_1(input: string): number
function day_09_2(input: string): number
function day_10_1(input: string): number
function day_10_2(input: string): number
function day_11_1(input: string): number
function day_11_2(input: string): number
function day_12_1(input: string): number
function day_12_2(input: string): number
function day_13_1(input: string): number
function day_13_2(input: string): number
function day_14_1(input: string): number
function day_14_2(input: string): number
function day_15_1(input: string): number
function day_15_2(input: string): number
function day_16_1(input: string): number
function day_16_2(input: string): number
function day_17_1(input: string): number
function day_17_2(input: string): number
function day_18_1(input: string): number
function day_18_2(input: string): number
function day_19_1(input: string): number
function day_19_2(input: string): number
function day_20_1(input: string): number
function day_20_2(input: string): number
function day_21_1(input: string): number
function day_21_2(input: string): number
function day_22_1(input: string): number
function day_22_2(input: string): number
function day_23_1(input: string): number
function day_23_2(input: string): number
function day_24_1(input: string): number
function day_24_2(input: string): number
function day_25_1(input: string): number
function day_25_2(input: string): number

function println(message: string)

function day_err(input: string): number {
    println("invalid day or level")
    return 0
}

const day = 1
const lvl = 1

const fn = switch (day) {
    case 1  -> lvl == 1 ? day_01_1 : lvl == 2 ? day_01_2 : day_err
    case 2  -> lvl == 1 ? day_02_1 : lvl == 2 ? day_02_2 : day_err
    case 3  -> lvl == 1 ? day_03_1 : lvl == 2 ? day_03_2 : day_err
    case 4  -> lvl == 1 ? day_04_1 : lvl == 2 ? day_04_2 : day_err
    case 5  -> lvl == 1 ? day_05_1 : lvl == 2 ? day_05_2 : day_err
    case 6  -> lvl == 1 ? day_06_1 : lvl == 2 ? day_06_2 : day_err
    case 7  -> lvl == 1 ? day_07_1 : lvl == 2 ? day_07_2 : day_err
    case 8  -> lvl == 1 ? day_08_1 : lvl == 2 ? day_08_2 : day_err
    case 9  -> lvl == 1 ? day_09_1 : lvl == 2 ? day_09_2 : day_err
    case 10 -> lvl == 1 ? day_10_1 : lvl == 2 ? day_10_2 : day_err
    case 11 -> lvl == 1 ? day_11_1 : lvl == 2 ? day_11_2 : day_err
    case 12 -> lvl == 1 ? day_12_1 : lvl == 2 ? day_12_2 : day_err
    case 13 -> lvl == 1 ? day_13_1 : lvl == 2 ? day_13_2 : day_err
    case 14 -> lvl == 1 ? day_14_1 : lvl == 2 ? day_14_2 : day_err
    case 15 -> lvl == 1 ? day_15_1 : lvl == 2 ? day_15_2 : day_err
    case 16 -> lvl == 1 ? day_16_1 : lvl == 2 ? day_16_2 : day_err
    case 17 -> lvl == 1 ? day_17_1 : lvl == 2 ? day_17_2 : day_err
    case 18 -> lvl == 1 ? day_18_1 : lvl == 2 ? day_18_2 : day_err
    case 19 -> lvl == 1 ? day_19_1 : lvl == 2 ? day_19_2 : day_err
    case 20 -> lvl == 1 ? day_20_1 : lvl == 2 ? day_20_2 : day_err
    case 21 -> lvl == 1 ? day_21_1 : lvl == 2 ? day_21_2 : day_err
    case 22 -> lvl == 1 ? day_22_1 : lvl == 2 ? day_22_2 : day_err
    case 23 -> lvl == 1 ? day_23_1 : lvl == 2 ? day_23_2 : day_err
    case 24 -> lvl == 1 ? day_24_1 : lvl == 2 ? day_24_2 : day_err
    case 25 -> lvl == 1 ? day_25_1 : lvl == 2 ? day_25_2 : day_err
    default -> day_err
}

const result = fn(input)
println($"result: {result}")
