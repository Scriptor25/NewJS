extern println(message: i8[])

function fib(n: u32): u32 {
    let a = 0u32
    let b = 1u32
    for (let i = 1u32; i < n; ++i) {
        let c = a + b
        a = b
        b = c
    }
    return b
}

let n = 10u32
let f = fib(n)
println($"fib({n}) = {f}")
