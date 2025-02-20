#infinity "(1.0:f64/0.0:f64)"
#pi "(3.1415926535897932385:f64)"

extern let rand_max: i32
extern function rand(): i32

function to_radians(degrees: f64): f64 {
    return degrees * pi / 180.0
}

function random(): f64 {
    return rand() / (rand_max + 1.0)
}

function random_range(min: f64, max: f64): f64 {
    return min + (max - min) * random()
}

/*
function<T> min(a: T, b: T): T {
    return a < b ? a : b
}

function<T> max(a: T, b: T): T {
    return a > b ? a : b
}
*/
