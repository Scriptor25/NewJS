import common from "./common.njs"

class interval {
    size(const &{ min, max }: interval): f64 {
        return max - min
    },

    contains(const &{ min, max }: interval, x: f64): u1 {
        return min <= x && x <= max
    },

    surrounds(const &{ min, max }: interval, x: f64): u1 {
        return min < x && x < max
    },

    clamp(const &{ min, max }: interval, x: f64): f64 {
        return x < min
               ? min
               : x > max
               ? max
               : x
    },

    expand(const &{ min, max }: interval, delta: f64): interval {
        const padding = delta / 2
        return {
            min: min - padding,
            max: max + padding,
        }
    },

    min: f64,
    max: f64,
}

export function combine(const &a: interval, const &b: interval): interval {
    const min = a.min <= b.min ? a.min : b.min
    const max = a.max >= b.max ? a.max : b.max
    return { min, max }
}

export function operator+(const &{ min, max }: interval, offset: f64): interval {
    return { min: min + offset, max: max + offset }
}

export function operator+(offset: f64, const &i: interval): interval {
    return i + offset
}

export const empty: interval     = { min:  infinity, max: -infinity }
export const universe: interval  = { min: -infinity, max:  infinity }
export const intensity: interval = { min:     0.000, max:     0.999 }
export const unit: interval = { min: 0, max: 1 }
