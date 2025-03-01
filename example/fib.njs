extern function println(message: i8[const])

export function fib(n: u32): u32 {
    let a: u32 = 0
    let b: u32 = 1
    for (let i: u32 = 1; i < n; ++i) {
        const c = a + b
        a = b
        b = c
    }
    return b
}

function<T> gen_fib(n: T): T {
    let a: T = 0
    let b: T = 0
    for (let i: T = 1; i < n; ++i) {
        const c = a + b
        a = b
        b = c
    }
    return b
}

const n: u32 = 10
const f = fib(n)
println(f"fib({n}) = {f}")

{
    const n: u64 = 10
    const f = gen_fib<u64>(n)
    println(f"fib({n}) = {f}")
}
