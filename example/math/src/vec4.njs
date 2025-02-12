type vec4 = f64[4]

function dot(l: vec4, r: vec4): f64 {
    return l[0] * r[0] + l[1] * r[1] + l[2] * r[2] + l[3] * r[3]
}
