type vec4 = {
    e0: f64,
    e1: f64,
    e2: f64,
    e3: f64,
}

function zero(): vec4 {
    return {
        e0: 0.0,
        e1: 0.0,
        e2: 0.0,
        e3: 0.0,
    }
}

function new(e0: f64, e1: f64, e2: f64, e3: f64): vec4 {
    return {
        e0,
        e1,
        e2,
        e3,
    }
}

function operator[(self: vec4&, index: i64): f64& {
    if (index == 0)
        return self.e0
    if (index == 1)
        return self.e1
    if (index == 2)
        return self.e2
    if (index == 3)
        return self.e3
    return *(0 as f64[])
}

function operator[(self: vec4, index: i64): f64 {
    if (index == 0)
        return self.e0
    if (index == 1)
        return self.e1
    if (index == 2)
        return self.e2
    if (index == 3)
        return self.e3
    return 0
}

function dot(l: vec4, r: vec4): f64 {
    return l[0] * r[0] + l[1] * r[1] + l[2] * r[2] + l[3] * r[3]
}
