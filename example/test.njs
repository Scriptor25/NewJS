extern println(message: i8[])

type User = {
    name: i8[],
    surname: i8[],
    age: u32,
    type: i8[],
}

const u1: User = {
    name: "Felix",
    surname: "Schreiber",
    age: 18,
    type: "App Development"
}

function printUser({ name: n, surname: s, age: a, type: t }: User) {
    println($"{n} {s} {a} {t}")
}

printUser(u1)

function recurse(n: u32): u32 {
    if (n <= 1) return n
    return recurse(n - 1) + recurse(n - 2)
}

const n = 10u32
const r = recurse(n)
println($"recurse({n}) = {r}")

const a1: u32[3] = [1, 2, 3]
const t1 = ["Felix", u1, a1]

function printArray(array: u32[3]) {
    println($"{array}")
}

printArray(a1)

for (let i = 0u32; i < 3; ++i)
    println($"a1[{i}] = {a1[i]}")

function foo(): (): u32 {
    function bar(): u32 {
        return 123
    }
    return bar
}

println($"foo()() = {foo()()}")

const [name1, {name, surname, age, type}] = ["Felix", u1]

println($"{name1} {name} {surname} {age} {type} {[4, 5, 6]}")
println($"t1 = {t1}")
