function println(message: string)

function fib(n: number): number {
    let a = 0
    let b = 1
    for (let i = 1; i < n; ++i) {
        const c = a + b
        a = b
        b = c
    }
    return b
}

const n = 10
const f = fib(n)
println($"fib({n}) = {f}")
