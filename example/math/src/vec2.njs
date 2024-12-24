type vec2 = f64[2]

extern sqrt(x: f64): f64

function zero(): vec2 {
    return [ 0.f64, 0.f64 ]
}

function scalar(s: f64): vec2 {
    return [ s, s ]
}

function new(e0: f64, e1: f64): vec2 {
    return [ e0, e1 ]
}

function len(self: vec2): f64 {
    return sqrt(self[0u64] * self[0u64] + self[1u64] * self[1u64])
}

operator-(self: vec2): vec2 {
    return new(-self[0u64], -self[1u64])
}

operator+=(self: vec2&, v: vec2): vec2& {
    self[0u64] += v[0u64]
    self[1u64] += v[1u64]
    return self
}

operator-=(self: vec2&, v: vec2): vec2& {
    self[0u64] -= v[0u64]
    self[1u64] -= v[1u64]
    return self
}

operator*=(self: vec2&, s: f64): vec2& {
    self[0u64] *= s
    self[1u64] *= s
    return self
}

operator/=(self: vec2&, s: f64): vec2& {
    return self *= (1.f64 / s)
}

operator+(l: vec2, r: vec2): vec2 {
    return new(l[0u64] + r[0u64], l[1u64] + r[1u64])
}

operator-(l: vec2, r: vec2): vec2 {
    return new(l[0u64] - r[0u64], l[1u64] - r[1u64])
}

operator*(l: vec2, r: f64): vec2 {
    return new(l[0u64] * r, l[1u64] * r)
}

operator*(l: f64, r: vec2): vec2 {
    return r * l
}

operator/(l: vec2, r: f64): vec2 {
    return l * (1.f64 / r)
}
