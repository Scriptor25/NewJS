import common from "./common.njs"

type interval = {
    min: f64,
    max: f64,
}

export const empty: interval = { min: infinity, max: -infinity }
export const universe: interval = { min: -infinity, max: infinity }
export const intensity: interval = { min: 0.000, max: 0.999 }

export function size(const &self: interval): f64 {
    return self.max - self.min
}

export function contains(const &self: interval, x: f64): u1 {
    return self.min <= x && x <= self.max
}

export function surrounds(const &self: interval, x: f64): u1 {
    return self.min < x && x < self.max
}

export function clamp(const &self: interval, x: f64): f64 {
    return x < self.min
           ? self.min
           : x > self.max
           ? self.max
           : x
}
