function println(...)

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
    if (n <= 1) return n
    return recurse(n - 1) + recurse(n - 2)
}

const n = 10
const r = recurse(10)
println($"recurse({n}) = {r}")

const t1 = ["Felix", u1]
const a1 = [1, 2, 3]

function printArray(array: number[]) {
    println(array)
}

printArray(t1)
printArray(a1)

function foo(x: number): (): number {
    function bar(): number {
        return x
    }
    return bar
}
