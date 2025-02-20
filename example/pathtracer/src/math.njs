import common from "./common.njs"

extern function sqrt(x: f64): f64
extern function fabs(x: f64): f64
extern function fmin(a: f64, b: f64): f64

type vec3 = f64[3]
type point3 = vec3

function operator-(self: vec3&): vec3 {
    return [
        -self[0],
        -self[1],
        -self[2],
    ]
}

function operator-(v: vec3): vec3 {
    return [
        -v[0],
        -v[1],
        -v[2],
    ]
}

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

function near_zero(v: vec3): u1 {
    const s = 1e-8
    return (fabs(v[0]) < s)
        && (fabs(v[1]) < s)
        && (fabs(v[2]) < s)
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
    return a[0] * b[0]
         + a[1] * b[1]
         + a[2] * b[2]
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

function random_vector(): vec3 {
    return [
        common.random(),
        common.random(),
        common.random(),
    ]
}

function random_range_vector(min: f64, max: f64): vec3 {
    return [
        common.random_range(min, max),
        common.random_range(min, max),
        common.random_range(min, max),
    ]
}

function random_unit_vector(): vec3 {
    for (;;) {
        const p = random_range_vector(-1, 1)
        const len_sq = length_squared(p)
        if (1e-160 < len_sq && len_sq <= 1)
            return p / sqrt(len_sq)
    }
}

function random_on_hemisphere(normal: vec3): vec3 {
    const on_unit_sphere = random_unit_vector()
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere
    return -on_unit_sphere
}

function random_in_unit_disk(): vec3 {
    for (;;) {
        const p = [
            common.random_range(-1, 1),
            common.random_range(-1, 1),
            0.0,
        ]:vec3
        if (length_squared(p) < 1.0)
            return p
    }
}

function reflect(v: vec3, n: vec3): vec3 {
    return v - 2.0 * dot(v, n) * n
}

function refract(v: vec3, n: vec3, eta: f64): vec3 {
    const cos_theta = fmin(dot(-v, n), 1.0)
    const r_out_perp = eta * (v + cos_theta * n)
    const r_out_para = -sqrt(fabs(1.0 - length_squared(r_out_perp))) * n
    return r_out_perp + r_out_para
}
