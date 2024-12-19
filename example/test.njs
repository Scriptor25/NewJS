extern println(message: i8[])

type User = {
    name: i8[],
    surname: i8[],
    age: u32,
    type: i8[],
}

let u1: User = {
    name: "Felix",
    surname: "Schreiber",
    age: 18u32,
    type: "App Development"
}

function printUser({ name: n, surname: s, age: a, type: t }: User&) {
    println($" - {n} {s} ({a}), {t}")
}

println("IT WORKS!")
printUser(u1)

function recurse(n: u32): u32 {
    if (n <= 1u32) return n
    return recurse(n - 1u32) + recurse(n - 2u32)
}

let n = 10u32
let r = recurse(n)
println($"recurse({n}) = {r}")

let a1 = [1u32, 2u32, 3u32]
let t1 = ["Felix", u1, a1]

function printArray(array: u32[3]) {
    println($"{array}")
}

printArray(a1)

for (let i: u32; i < 3u32; ++i)
    println($"a1[{i}] = {a1[i]}")

function foo(): (): u32 {
    function bar(): u32 {
        return 123u32
    }
    return bar
}

println($"foo() = {foo()}")
println($"foo()() = {foo()()}")

let [name1, {name, surname, age, type}] = ["Felix", u1]

println($"{name1} {name} {surname} {age} {type} {[4u32, 5u32, 6u32]}")
println($"t1 = {t1}")
