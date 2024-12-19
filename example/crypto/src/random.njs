extern rand(): u32

function random_f64(): f64 {
    return rand() as f64 / 32767u32 as f64
}

function random(): u64 {
    return ((~0u64) as f64 * random_f64()) as u64 + 1u64
}
