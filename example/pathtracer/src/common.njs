#infinity "(1.0:f64/0.0:f64)"
#pi "(3.1415926535897932385:f64)"

function deg_to_rad(deg: f64): f64 {
    return deg * pi / 180.0
}

function<T> min(a: T, b: T): T {
    return a < b ? a : b
}
