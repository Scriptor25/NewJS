type vec2 = f64[2]

extern sqrt(x: f64): f64
extern println(x: i8[])

function vec2(x: f64, y: f64): vec2 {
    return [x, y]
}

function len(v: vec2): f64 {
    return sqrt(v[0u64] * v[0u64] + v[1u64] * v[1u64])
}

operator+(l: vec2, r: vec2): vec2 {
    return [l[0u64] + r[0u64], l[1u64] + r[1u64]]
}

operator-(l: vec2, r: vec2): vec2 {
    return [l[0u64] - r[0u64], l[1u64] - r[1u64]]
}

operator*(l: vec2, r: f64): vec2 {
    return [l[0u64] * r, l[1u64] * r]
}

operator*(l: f64, r: vec2): vec2 {
    return r * l
}

operator/(l: vec2, r: f64): vec2 {
    return l * (1.0f64 / r)
}

let a = vec2(1.2f64, 3.4f64)
let b: vec2 = [10.0f64, -123.f64]
let c = [-1.5f64, 2.3f64]
let d = 6.4f64

println($"{a} + {b} = {a + b}")
println($"{a} - {b} = {a - b}")
println($"{c} * {d} = {c * d}")
println($"{d} * {c} = {d * c}")
println($"{c} / {d} = {c / d}")
