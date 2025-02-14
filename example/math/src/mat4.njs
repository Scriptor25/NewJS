import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"
import vec4 from "./vec4.njs"

type mat4 = f64[4][4]

function new(): mat4 {
    return [
        [1., 0., 0., 0.],
        [0., 1., 0., 0.],
        [0., 0., 1., 0.],
        [0., 0., 0., 1.],
    ]
}

function transpose(self: mat4&): mat4 {
    let m: mat4
    for (let j = 0; j < 4; ++j)
        for (let i = 0; i < 4; ++i)
            m[j][i] = self[i][j]
    return m
}

function operator=(self: mat4&, other: mat4): mat4& {
    for (let j = 0; j < 4; ++j)
        for (let i = 0; i < 4; ++i)
            self[j][i] = other[j][i]
    return self
}

function operator*=(self: mat4&, other: mat4): mat4& {
    const s = self
    const t = transpose(other)
    for (let j = 0; j < 4; ++j)
        for (let i = 0; i < 4; ++i)
            self[j][i] = vec4.dot(s[j], t[i])
    return self
}

function operator*(l: vec4&, r: mat4&): vec4 {
    let v = vec4.zero()
    let t = transpose(r)
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l, t[i])
    return v
}

function operator*(l: vec4, r: mat4&): vec4 {
    let v = vec4.zero()
    let t = transpose(r)
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l, t[i])
    return v
}

function operator*(l: mat4&, r: vec4&): vec4 {
    let v = vec4.zero()
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l[i], r)
    return v
}

function operator*(l: mat4&, r: vec4): vec4 {
    let v = vec4.zero()
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l[i], r)
    return v
}

function identity(self: mat4&): mat4& {
    return self = new()
}

function translation(v: vec3): mat4 {
    let m = new()
    m[0][3] = v[0]
    m[1][3] = v[1]
    m[2][3] = v[2]
    return m
}

function scale(v: vec3): mat4 {
    let m = new()
    m[0][0] = v[0]
    m[1][1] = v[1]
    m[2][2] = v[2]
    return m
}
