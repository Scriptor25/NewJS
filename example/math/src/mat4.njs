import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"

type mat4 = f64[4][4]

function new(): mat4 {
    return [
        [1.f64, 0.f64, 0.f64, 0.f64],
        [0.f64, 1.f64, 0.f64, 0.f64],
        [0.f64, 0.f64, 1.f64, 0.f64],
        [0.f64, 0.f64, 0.f64, 1.f64],
    ]
}

function ident(self: mat4&): mat4& {
    self[0u64][0u64] =
    self[1u64][1u64] =
    self[2u64][2u64] =
    self[3u64][3u64] = 1.f64
    return self
}

function translate(self: mat4&, v: vec3): mat4& {
    self[0u64][3u64] += v[0u64]
    self[1u64][3u64] += v[1u64]
    self[2u64][3u64] += v[2u64]
    return self
}

function scale(self: mat4&, v: vec3): mat4& {
    self[0u64][0u64] += v[0u64]
    self[1u64][1u64] += v[1u64]
    self[2u64][2u64] += v[2u64]
    return self
}
