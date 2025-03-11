import common from "./common.njs"

type interval = {
    size: (const &interval) => f64,
    contains: (const &interval, f64) => u1,
    surrounds: (const &interval, f64) => u1,
    clamp: (const &interval, f64) => f64,

    min: f64,
    max: f64,
}

function size(const &self: interval): f64 {
    return self.max - self.min
}

function contains(const &self: interval, x: f64): u1 {
    return self.min <= x && x <= self.max
}

function surrounds(const &self: interval, x: f64): u1 {
    return self.min < x && x < self.max
}

function clamp(const &self: interval, x: f64): f64 {
    return x < self.min
           ? self.min
           : x > self.max
           ? self.max
           : x
}

export function create(min: f64, max: f64): interval {
    return {
        size,
        contains,
        surrounds,
        clamp,

        min,
        max,
    }
}

export let empty: interval     = create( infinity, -infinity)
export let universe: interval  = create(-infinity,  infinity)
export let intensity: interval = create(    0.000,     0.999)
