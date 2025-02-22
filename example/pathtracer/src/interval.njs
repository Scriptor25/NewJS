import common from "./common.njs"

type interval = {
    min: f64,
    max: f64,
}

export function size(i: interval): f64 {
    return i.max - i.min
}

export function contains(i: interval, x: f64): u1 {
    return i.min <= x && x <= i.max
}

export function surrounds(i: interval, x: f64): u1 {
    return i.min < x && x < i.max
}

export function clamp(i: interval, x: f64): f64 {
    return x < i.min
           ? i.min
           : x > i.max
           ? i.max
           : x
}
