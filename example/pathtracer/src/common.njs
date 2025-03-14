#infinity "(1.0 / 0.0)"
#pi       "(3.1415926535897932385)"

#MAX(A, B) "((%A) > (%B) ? (%A) : (%B))"
#MIN(A, B) "((%A) < (%B) ? (%A) : (%B))"
#PAIR(F, S) "{ fst: %F, snd: %S }"

extern let rand_max: i32
extern function rand(): i32

export function to_radians(degrees: f64): f64 {
    return degrees * pi / 180.0
}

export function random(): f64 {
    return rand() / (rand_max + 1.0)
}

export function random_range(min: f64, max: f64): f64 {
    return min + (max - min) * random()
}
