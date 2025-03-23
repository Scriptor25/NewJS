import common from "./common.njs"

extern function sqrt(x: f64): f64
extern function fabs(x: f64): f64
extern function fmin(a: f64, b: f64): f64

class vec3 {
    length_squared(const &{ e: [x, y, z] }: vec3): f64 {
        return x * x + y * y + z * z
    },

    length(const &self: vec3): f64 {
        return sqrt(self.length_squared())
    },

    near_zero(const &{ e: [x, y, z] }: vec3): u1 {
        const s = 1e-8
        return (fabs(x) < s)
            && (fabs(y) < s)
            && (fabs(z) < s)
    },

    e: f64[3],
}

type point3 = vec3

export function operator[](&{ e }: vec3, index: i64): &f64 {
    return e[index]
}

export function operator[](const &{ e }: vec3, index: i64): const &f64 {
    return e[index]
}

export function operator-(const &{ e: [x, y, z] }: vec3): vec3 {
    return { e: [
        -x,
        -y,
        -z,
    ] }
}

export function operator+=(&self: vec3, const &{ e: [vx, vy, vz] }: vec3): &vec3 {
    self.e[0] += vx
    self.e[1] += vy
    self.e[2] += vz
    return self
}

export function operator*=(&self: vec3, t: f64): &vec3 {
    self.e[0] *= t
    self.e[1] *= t
    self.e[2] *= t
    return self
}

export function operator/=(&self: vec3, other: f64): &vec3 {
    return self *= 1.0 / other
}

export function operator+(const &{ e: [ax, ay, az] }: vec3, const &{ e: [bx, by, bz] }: vec3): vec3 {
    return { e: [
        ax + bx,
        ay + by,
        az + bz,
    ] }
}

export function operator-(const &{ e: [ax, ay, az] }: vec3, const &{ e: [bx, by, bz] }: vec3): vec3 {
    return { e: [
        ax - bx,
        ay - by,
        az - bz,
    ] }
}

export function operator*(const &{ e: [ax, ay, az] }: vec3, const &{ e: [bx, by, bz] }: vec3): vec3 {
    return { e: [
        ax * bx,
        ay * by,
        az * bz,
    ] }
}

export function operator*(a: f64, const &{ e: [bx, by, bz] }: vec3): vec3 {
    return { e: [
        a * bx,
        a * by,
        a * bz,
    ] }
}

export function operator*(const &a: vec3, b: f64): vec3 {
    return b * a
}

export function operator/(const &a: vec3, b: f64): vec3 {
    return (1 / b) * a
}

export function dot(const &{ e: [ax, ay, az] }: vec3, const &{ e: [bx, by, bz] }: vec3): f64 {
    return ax * bx + ay * by + az * bz
}

export function cross(const &{ e: [ax, ay, az] }: vec3, const &{ e: [bx, by, bz] }: vec3): vec3 {
    return { e: [
        ay * bz - az * by,
        az * bx - ax * bz,
        ax * by - ay * bx,
    ] }
}

export function unit_vector(const &self: vec3): vec3 {
    return self / self.length()
}

export function part_vector(const &self: vec3): vec3 {
    return self / self.length_squared()
}

export function random_vector(): vec3 {
    return { e: [
        common.random(),
        common.random(),
        common.random(),
    ] }
}

export function random_range_vector(min: f64, max: f64): vec3 {
    return { e: [
        common.random_range(min, max),
        common.random_range(min, max),
        common.random_range(min, max),
    ] }
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
        const p = { e: [
            common.random_range(-1, 1),
            common.random_range(-1, 1),
            0,
        ] }:vec3
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
