extern function sqrt(x: f64): f64

type vec3 = f64[3]
type point3 = vec3

function operator-(self: vec3&): vec3 { return [ -self[0], -self[1], -self[2] ] }

function operator+=(self: vec3&, other: vec3): vec3& {
    self[0] += other[0]
    self[1] += other[1]
    self[2] += other[2]
    return self
}

function operator*=(self: vec3&, other: f64): vec3& {
    self[0] *= other
    self[1] *= other
    self[2] *= other
    return self
}

function operator/=(self: vec3&, other: f64): vec3& {
    return self *= 1.0 / other
}

function length_squared(v: vec3): f64 {
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2]
}

function length(v: vec3): f64 {
    return sqrt(length_squared(v))
}

function operator+(a: vec3, b: vec3): vec3 {
    return [
        a[0] + b[0],
        a[1] + b[1],
        a[2] + b[2],
    ]
}

function operator-(a: vec3, b: vec3): vec3 {
    return [
        a[0] - b[0],
        a[1] - b[1],
        a[2] - b[2],
    ]
}

function operator*(a: vec3, b: vec3): vec3 {
    return [
        a[0] * b[0],
        a[1] * b[1],
        a[2] * b[2],
    ]
}

function operator*(a: f64, b: vec3): vec3 {
    return [
        a * b[0],
        a * b[1],
        a * b[2],
    ]
}

function operator*(a: vec3, b: f64): vec3 {
    return b * a
}

function operator/(a: vec3, b: f64): vec3 {
    return (1 / b) * a
}

function dot(a: vec3, b: vec3): f64 {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]
}

function cross(a: vec3, b: vec3): vec3 {
    return [
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    ]
}

function unit_vector(v: vec3): vec3 {
    return v / length(v)
}
