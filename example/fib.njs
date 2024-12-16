extern println(message: i8[])

function fib(n: u32): u32 {
    let a = 0u32
    let b = 1u32
    for (let i = 1u32; i < n; ++i) {
        const c = a + b
        a = b
        b = c
    }
    return b
}

const n = 10u32
const f = fib(n)
println($"fib({n}) = {f}")
