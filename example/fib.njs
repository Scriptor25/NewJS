extern function println(message: i8[])

export function fib(n: u32): u32 {
    let a: u32 = 0
    let b: u32 = 1
    for (let i: u32 = 1; i < n; ++i) {
        let c = a + b
        a = b
        b = c
    }
    return b
}

const n: u32 = 10
const f = fib(n)
println($"fib({n}) = {f}")
