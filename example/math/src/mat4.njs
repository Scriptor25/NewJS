import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"

type mat4 = f64[4][4]

function new(): mat4 {
    return [
        [1., 0., 0., 0.],
        [0., 1., 0., 0.],
        [0., 0., 1., 0.],
        [0., 0., 0., 1.],
    ]
}

function ident(self: mat4&): mat4& {
    self[0][0] =
    self[1][1] =
    self[2][2] =
    self[3][3] = 1.
    return self
}

function translate(self: mat4&, v: vec3): mat4& {
    self[0][3] += v[0]
    self[1][3] += v[1]
    self[2][3] += v[2]
    return self
}

function scale(self: mat4&, v: vec3): mat4& {
    self[0][0] += v[0]
    self[1][1] += v[1]
    self[2][2] += v[2]
    return self
}
