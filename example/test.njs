import { fib } from "./fib.njs"

extern function println(message: i8[])
extern function malloc(n: u64): void[]
extern function realloc(block: void[], n: u64): void[]
extern function free(block: void[])

#PRINT(X: expr)  "println(f\"#X = {#X}\")"
#ASSERT(X: expr) "assert(%X, \"#X\")"

function assert(x: u1, string: i8[const]) { if (!x) println(f"assertion '{string}' failed") }

type user_t = {
    name: i8[],
    surname: i8[],
    age: u32,
    job: i8[],
}

const user1: user_t = {
    name: "Max",
    surname: "Mustermann",
    age: 18,
    job: "Programmer"
}

PRINT(user1)
PRINT(println)

function printUser(const &{ name: n, surname: s, age: a, job: t }: user_t) {
    println(f" - {n} {s} ({a}), {t}")
}

class adder {
    call(const &self: adder): u32 {
        return self.a + self.b
    },

    a: u32,
    b: u32,
}

function operator()(const &self: adder): u32 {
    return self.call()
}

const add: adder = { a: 123, b: 321 }
println(f"add() = {add.a} + {add.b} = {add.a + add.b} = {add()}")

println("IT WORKS!")
printUser(user1)

function recurse(n: u32): u32 {
    return (n <= 1)
        ? n
        : recurse(n - 1) + recurse(n - 2)
}

const n: u32 = 10
const r = recurse(n)
println(f"recurse({n}) = {r}")

println(f"fib({n}) = {fib(n)}")

const a1: u32[3] = [1, 2, 3]
const t1 = ["Max", user1, a1]

function printArray(array: u32[3]) {
    println(f"{array}")
}

printArray(a1)

for (let i = 0; i < 3; ++i)
    println(f"a1[{i}] = {a1[i]}")

/*
function foo(a: u32, b: u32, &c: u32): lambda[a: u32, b: u32, &c: u32]<() => u32> {
    const bar = $[ a, b, &r: c ]: u32 {
        return r = a + b
    }
    return bar
}

let foo_res: u32
const foo_123 = foo(123, 321, foo_res)
println(f"foo(123, 321, foo_res) = {typeof(foo_123)}")
println(f"foo(123, 321, foo_res)() = {foo_123()}")
println(f"foo_res = {foo_res}")
*/

const [name1, {name, surname, age, job}] = ["Max", user1]

println(f"{name1} {name} {surname} {age} {job} {[4:u32, 5:u32, 6:u32]}")
println(f"t1 = {t1}")

for (let i = 0; i < 64; ++i)
    println(f"i = {(2 ** i) as u64}")

for (let j: u64; j < 5; ++j)
    for (let i: u64; i < 5; ++i) {
        if (i == j)
            continue
        ASSERT(i != j && "i must not be equal to j")
    }

for (let x: u64; x < 10; ++x) {
    if (x > 5)
        break
    ASSERT(x <= 5 && "x must not be greater than 5")
}

switch (0) {
    case 0 -> break
    case 1 {
        println("Hello World!")
    }
    case 2 {
        return 123
    }
    default {
        if (1 == 2)
            break
        return 456
    }
}

{
    const foo = 123
    const bar = 456
    const a = foo ?? bar ?? 789
}

{
    const a: i32 = 5
    const b: i32 = 3
    const result = asm volatile("addl $2, $1" : "=r"<i32> : "r"(a), "r"(b))
    println(f"asm for {a} + {b} = {result}")
}
