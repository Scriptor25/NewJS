type vec2 = f64[2]

extern function sqrt(x: f64): f64

export function zero(): vec2 {
    return [ 0., 0. ]
}

export function scalar(s: f64): vec2 {
    return [ s, s ]
}

export function new(e0: f64, e1: f64): vec2 {
    return [ e0, e1 ]
}

export function len(&self: vec2): f64 {
    return sqrt(self[0] * self[0] + self[1] * self[1])
}

export function operator++(&self: vec2): vec2& {
    ++self[0] ; ++self[1]
    return self
}

export function operator++(&self: vec2, ...): vec2 {
    const old = self
    self[0]++ ; self[1]++
    return old
}

export function operator--(&self: vec2): vec2& {
    --self[0] ; --self[1]
    return self
}

export function operator--(&self: vec2, ...): vec2 {
    const old = self
    self[0]-- ; self[1]--
    return old
}

export function operator-(&self: vec2): vec2 {
    return new(-self[0], -self[1])
}

export function operator-(self: vec2): vec2 {
    return new(-self[0], -self[1])
}

export function operator=(&self: vec2, &v: vec2): vec2& {
    self[0] = v[0]
    self[1] = v[1]
    return self
}

export function operator=(&self: vec2, v: vec2): vec2& {
    self[0] = v[0]
    self[1] = v[1]
    return self
}

export function operator=(&self: vec2, s: f64): vec2& {
    self[0] = s
    self[1] = s
    return self
}

export function operator+=(&self: vec2, &v: vec2): vec2& {
    self[0] += v[0]
    self[1] += v[1]
    return self
}

export function operator+=(&self: vec2, v: vec2): vec2& {
    self[0] += v[0]
    self[1] += v[1]
    return self
}

export function operator-=(&self: vec2, &v: vec2): vec2& {
    self[0] -= v[0]
    self[1] -= v[1]
    return self
}

export function operator-=(&self: vec2, v: vec2): vec2& {
    self[0] -= v[0]
    self[1] -= v[1]
    return self
}

export function operator*=(&self: vec2, s: f64): vec2& {
    self[0] *= s
    self[1] *= s
    return self
}

export function operator/=(&self: vec2, s: f64): vec2& {
    return self *= (1. / s)
}

export function operator+(l: vec2, r: vec2): vec2 {
    return new(l[0] + r[0], l[1] + r[1])
}

export function operator-(l: vec2, r: vec2): vec2 {
    return new(l[0] - r[0], l[1] - r[1])
}

export function operator*(l: vec2, r: f64): vec2 {
    return new(l[0] * r, l[1] * r)
}

export function operator*(l: f64, r: vec2): vec2 {
    return r * l
}

export function operator/(l: vec2, r: f64): vec2 {
    return l * (1. / r)
}
