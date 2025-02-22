import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"
import vec4 from "./vec4.njs"

type mat4 = {
    r0: vec4,
    r1: vec4,
    r2: vec4,
    r3: vec4,
}

export function new(): mat4 {
    return {
        r0: vec4.new(1.0, 0.0, 0.0, 0.0),
        r1: vec4.new(0.0, 1.0, 0.0, 0.0),
        r2: vec4.new(0.0, 0.0, 1.0, 0.0),
        r3: vec4.new(0.0, 0.0, 0.0, 1.0),
    }
}

export function operator[(&self: mat4, index: i64): vec4& {
    if (index == 0)
        return self.r0
    if (index == 1)
        return self.r1
    if (index == 2)
        return self.r2
    if (index == 3)
        return self.r3
    return *(0 as vec4[])
}

export function operator[(const &self: mat4, index: i64): const vec4& {
    if (index == 0)
        return self.r0
    if (index == 1)
        return self.r1
    if (index == 2)
        return self.r2
    if (index == 3)
        return self.r3
    return *(0 as vec4[])
}

export function operator[(self: mat4, index: i64): vec4 {
    if (index == 0)
        return self.r0
    if (index == 1)
        return self.r1
    if (index == 2)
        return self.r2
    if (index == 3)
        return self.r3
    return vec4.zero()
}

export function transpose(const &self: mat4): mat4 {
    let m: mat4
    for (let j = 0; j < 4; ++j)
        for (let i = 0; i < 4; ++i)
            m[j][i] = self[i][j]
    return m
}

export function operator=(&self: mat4, other: mat4): mat4& {
    for (let j = 0; j < 4; ++j)
        for (let i = 0; i < 4; ++i)
            self[j][i] = other[j][i]
    return self
}

export function operator*=(&self: mat4, other: mat4): mat4& {
    const s = self
    const t = transpose(other)
    for (let j = 0; j < 4; ++j)
        for (let i = 0; i < 4; ++i)
            self[j][i] = vec4.dot(s[j], t[i])
    return self
}

export function operator*(const &l: vec4, const &r: mat4): vec4 {
    let v = vec4.zero()
    let t = transpose(r)
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l, t[i])
    return v
}

export function operator*(l: vec4, const &r: mat4): vec4 {
    let v = vec4.zero()
    let t = transpose(r)
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l, t[i])
    return v
}

export function operator*(const &l: mat4, const &r: vec4): vec4 {
    let v = vec4.zero()
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l[i], r)
    return v
}

export function operator*(const &l: mat4, r: vec4): vec4 {
    let v = vec4.zero()
    for (let i = 0; i < 4; ++i)
        v[i] = vec4.dot(l[i], r)
    return v
}

export function identity(&self: mat4): mat4& {
    return self = new()
}

export function translation(v: vec3): mat4 {
    let m = new()
    m[0][3] = v[0]
    m[1][3] = v[1]
    m[2][3] = v[2]
    return m
}

export function scale(v: vec3): mat4 {
    let m = new()
    m[0][0] = v[0]
    m[1][1] = v[1]
    m[2][2] = v[2]
    return m
}
