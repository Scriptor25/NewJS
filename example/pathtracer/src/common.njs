#infinity "(1.0 / 0.0)"
#pi       "(3.1415926535897932385)"

#MAX(A, B) "((%A) > (%B) ? (%A) : (%B))"
#MIN(A, B) "((%A) < (%B) ? (%A) : (%B))"
#PAIR(F, S) "{ fst: %F, snd: %S }"
#NEW(T: type, V: expr) "(&(*(malloc(sizeof<%T>) as %T[]) = %V))"

extern let rand_max: i32
extern function rand(): i32
extern function qsort(base: void[], num: u64, size: u64, cmp: (void[const], void[const]) => i32)

export function to_radians(degrees: f64): f64 {
    return degrees * pi / 180.0
}

export function random(): f64 {
    return rand() / (rand_max + 1.0)
}

export function random_range(min: f64, max: f64): f64 {
    return min + (max - min) * random()
}

export function random_int(min: i32, max: i32): i32 {
    return random_range(min, max + 1)
}

export function sort(base: void[], num: u64, size: u64, cmp: (void[const], void[const]) => i32) {
    qsort(base, num, size, cmp)
}
