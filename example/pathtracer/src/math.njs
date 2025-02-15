extern function sqrt(x: f64): f64

type vec3 = f64[3]
type point3 = vec3

function zero(): vec3 { return [ 0.0, 0.0, 0.0 ] }
function new(e0: f64, e1: f64, e2: f64): vec3 { return [ e0, e1, e2 ] }

function x(self: vec3&): f64& { return self[0] }
function y(self: vec3&): f64& { return self[1] }
function z(self: vec3&): f64& { return self[2] }

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

function length_squared(self: vec3&): f64 {
    return self[0] * self[0] + self[1] * self[1] + self[2] * self[2]
}

function length(self: vec3&): f64 {
    return sqrt(length_squared(self))
}

function operator+(a: vec3, b: vec3): vec3 {
    return [
        a[0] + b[0],
        a[1] + b[1],
        a[2] + b[2],
    ]
}
