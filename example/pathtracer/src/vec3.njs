import common from "./common.njs"

extern function sqrt(x: f64): f64
extern function fabs(x: f64): f64
extern function fmin(a: f64, b: f64): f64

type vec3 = {
    near_zero: (const &vec3) => u1,
    length_squared: (const &vec3) => f64,
    length: (const &vec3) => f64,

    e: f64[3],
}

type point3 = vec3

export function create(e0: f64, e1: f64, e2: f64): vec3

export function operator[(&self: vec3, index: i64): &f64 {
    return self.e[index]
}

export function operator[(const &self: vec3, index: i64): const &f64 {
    return self.e[index]
}

export function operator-(const &self: vec3): vec3 {
    return create(
        -self.e[0],
        -self.e[1],
        -self.e[2],
    )
}

export function operator+=(&self: vec3, const &other: vec3): &vec3 {
    self.e[0] += other.e[0]
    self.e[1] += other.e[1]
    self.e[2] += other.e[2]
    return self
}

export function operator*=(&self: vec3, other: f64): &vec3 {
    self.e[0] *= other
    self.e[1] *= other
    self.e[2] *= other
    return self
}

export function operator/=(&self: vec3, other: f64): &vec3 {
    return self *= 1.0 / other
}

export function operator+(const &a: vec3, const &b: vec3): vec3 {
    return create(
        a.e[0] + b.e[0],
        a.e[1] + b.e[1],
        a.e[2] + b.e[2],
    )
}

export function operator-(const &a: vec3, const &b: vec3): vec3 {
    return create(
        a.e[0] - b.e[0],
        a.e[1] - b.e[1],
        a.e[2] - b.e[2],
    )
}

export function operator*(const &a: vec3, const &b: vec3): vec3 {
    return create(
        a.e[0] * b.e[0],
        a.e[1] * b.e[1],
        a.e[2] * b.e[2],
    )
}

export function operator*(a: f64, const &b: vec3): vec3 {
    return create(
        a * b.e[0],
        a * b.e[1],
        a * b.e[2],
    )
}

export function operator*(const &a: vec3, b: f64): vec3 {
    return b * a
}

export function operator/(const &a: vec3, b: f64): vec3 {
    return (1 / b) * a
}

function length_squared(const &self: vec3): f64 {
    return self.e[0] * self.e[0] + self.e[1] * self.e[1] + self.e[2] * self.e[2]
}

function length(const &self: vec3): f64 {
    return sqrt(self.length_squared())
}

function near_zero(const &self: vec3): u1 {
    const s = 1e-8
    return (fabs(self.e[0]) < s)
        && (fabs(self.e[1]) < s)
        && (fabs(self.e[2]) < s)
}

export function dot(const &a: vec3, const &b: vec3): f64 {
    return a.e[0] * b.e[0]
         + a.e[1] * b.e[1]
         + a.e[2] * b.e[2]
}

export function cross(const &a: vec3, const &b: vec3): vec3 {
    return create(
        a.e[1] * b.e[2] - a.e[2] * b.e[1],
        a.e[2] * b.e[0] - a.e[0] * b.e[2],
        a.e[0] * b.e[1] - a.e[1] * b.e[0],
    )
}

export function unit_vector(const &v: vec3): vec3 {
    return v / length(v)
}

export function random_vector(): vec3 {
    return create(
        common.random(),
        common.random(),
        common.random(),
    )
}

export function random_range_vector(min: f64, max: f64): vec3 {
    return create(
        common.random_range(min, max),
        common.random_range(min, max),
        common.random_range(min, max),
    )
}

export function random_unit_vector(): vec3 {
    for (;;) {
        const p = random_range_vector(-1, 1)
        const len_sq = p.length_squared()
        if (1e-160 < len_sq && len_sq <= 1)
            return p / sqrt(len_sq)
    }
}

export function random_on_hemisphere(const &normal: vec3): vec3 {
    const on_unit_sphere = random_unit_vector()
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere
    return -on_unit_sphere
}

export function random_in_unit_disk(): vec3 {
    for (;;) {
        const p = [
            common.random_range(-1, 1),
            common.random_range(-1, 1),
            0.0,
        ]:vec3
        if (p.length_squared() < 1.0)
            return p
    }
}

export function reflect(const &v: vec3, const &n: vec3): vec3 {
    return v - 2.0 * dot(v, n) * n
}

export function refract(const &v: vec3, const &n: vec3, eta: f64): vec3 {
    const cos_theta = fmin(dot(-v, n), 1.0)
    const r_out_perp = eta * (v + cos_theta * n)
    const r_out_para = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n
    return r_out_perp + r_out_para
}

export function create(e0: f64, e1: f64, e2: f64): vec3 {
    return {
        near_zero,
        length_squared,
        length,

        e: [e0, e1, e2],
    }
}

export function empty(): vec3 {
    return {
        near_zero,
        length_squared,
        length,
    }
}
