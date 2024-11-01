function println() {}

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

type User = {
    name: string,
    surname: string,
    age: number,
    type: string,
}

const u1 = {
    name: "Felix",
    surname: "Schreiber",
    age: 18,
    type: "App Development"
}

function printUser({ name: n, surname: s, age: a, type: t }: User) {
    println(n, s, a, t)
}

printUser(u1)

function recurse(n: number): number {
    if (n < 2) return n
    return recurse(n - 1) + recurse(n - 2)
}

const r = recurse(10)
println($"recurse(10) = {r}")
