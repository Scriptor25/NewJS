import common from "./common.njs"
import vec3   from "./vec3.njs"

extern function floor(x: f64): f64
extern function fabs(x: f64): f64

function perlin_generate_perm(&p: i32[256]) {
    for (let i = 0; i < 256; ++i)
        p[i] = i

    for (let i = 256 - 1; i > 0; --i) {
        const target = common.random_int(0, i)
        const tmp = p[i]
        p[i] = p[target]
        p[target] = tmp
    }    
}

function trilinear_interp(c: vec3[2][2][2], u: f64, v: f64, w: f64): f64 {
    const uu = u * u * (3 - 2 * u)
    const vv = v * v * (3 - 2 * v)
    const ww = w * w * (3 - 2 * w)

    let accum = 0.0

    for (let i = 0; i < 2; ++i)
        for (let j = 0; j < 2; ++j)
            for (let k = 0; k < 2; ++k)
                accum += (i * uu + (1 - i) * (1 - uu))
                * (j * vv + (1 - j) * (1 - vv))
                * (k * ww + (1 - k) * (1 - ww))
                * vec3.dot(c[i][j][k], { e: [u - i, v - j, w - k] })
    return accum
}

class perlin {
    noise(const &{ perm_x, perm_y, perm_z, randvec }: perlin, const &{ e: [px, py, pz] }: point3): f64 {
        let u = px - floor(px)
        let v = py - floor(py)
        let w = pz - floor(pz)

        const i: i32 = floor(px)
        const j: i32 = floor(py)
        const k: i32 = floor(pz)

        let c: vec3[2][2][2]

        for (let di = 0; di < 2; ++di)
            for (let dj = 0; dj < 2; ++dj)
                for (let dk = 0; dk < 2; ++dk)
                    c[di][dj][dk] = randvec[
                        perm_x[(i + di) & 0xff] ^
                        perm_y[(j + dj) & 0xff] ^
                        perm_z[(k + dk) & 0xff]
                    ]

        return trilinear_interp(c, u, v, w)
    },

    turb(const &self: perlin, const &p: point3, depth: i32): f64 {
        let accum = 0.0
        let temp_p = p
        let weight = 1.0

        for (let i = 0; i < depth; ++i) {
            accum += weight * self.noise(temp_p)
            weight *= 0.5
            temp_p *= 2.0
        }

        return fabs(accum)
    },

    randvec: vec3[256],
    perm_x: i32[256],
    perm_y: i32[256],
    perm_z: i32[256],
}

export function create(): perlin {
    let p: perlin = {}
    
    for (let i = 0; i < 256; ++i)
        p.randvec[i] = vec3.unit_vector(vec3.random_range_vector(-1, 1))

    perlin_generate_perm(p.perm_x)
    perlin_generate_perm(p.perm_y)
    perlin_generate_perm(p.perm_z)

    return p
}
