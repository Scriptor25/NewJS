type vec4 = f64[4]

function zero(): vec4 {
    return [ 0., 0., 0., 0. ]
}

function new(e0: f64, e1: f64, e2: f64, e3: f64): vec4 {
    return [ e0, e1, e2, e3 ]
}

function dot(l: vec4, r: vec4): f64 {
    return l[0] * r[0] + l[1] * r[1] + l[2] * r[2] + l[3] * r[3]
}
