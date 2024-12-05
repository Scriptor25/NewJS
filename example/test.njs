extern println(message: string)

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
    println($"{n} {s} {a} {t}")
}

printUser(u1)

function recurse(n: number): number {
    if (n <= 1) return n
    return recurse(n - 1) + recurse(n - 2)
}

const n = 10
const r = recurse(n)
println($"recurse({n}) = {r}")

const a1 = [1, 2, 3]
const t1 = ["Felix", u1, a1]

function printArray(array: number[3]) {
    println($"{array}")
}

printArray(a1)

for (let i = 0; i < 3; ++i)
    println($"a1[{i}] = {a1[i]}")

function foo(): (): number {
    function bar(): number {
        return 123
    }
    return bar
}

println($"foo()() = {foo()()}")

const [name1, {name, surname, age, type}] = ["Felix", u1]

println($"{name1} {name} {surname} {age} {type} {[4, 5, 6]}")
println($"t1 = {t1}")
