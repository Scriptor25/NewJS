import { fib } from "./fib.njs"

extern function println(message: i8[])
extern function malloc(n: u64): void[]
extern function realloc(block: void[], n: u64): void[]
extern function free(block: void[])

#PRINT(X)  "println(f\"#X = {#X}\")"
#ASSERT(X) "assert(%X, \"#X\")"

function assert(x: u1, string: i8[const]) { if (!x) println(f"assertion '{string}' failed") }

type User = {
    name: i8[],
    surname: i8[],
    age: u32,
    job: i8[],
}

const user1: User = {
    name: "Max",
    surname: "Mustermann",
    age: 18,
    job: "Programmer"
}

PRINT(user1)
PRINT(println)

function printUser(const &{ name: n, surname: s, age: a, job: t }: User) {
    println(f" - {n} {s} ({a}), {t}")
}

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

function foo(): () => u32 {
    function bar(): u32 {
        return 123
    }
    return bar
}

println(f"foo() = {typeof(foo())}")
println(f"foo()() = {foo()()}")

const [name1, {name, surname, age, job}] = ["Max", user1]

println(f"{name1} {name} {surname} {age} {job} {[4:u32, 5:u32, 6:u32]}")
println(f"t1 = {t1}")

type<T> vec = {
    beg: T[],
    end: T[],
}

function<T> vec_new(n: u64): vec<T> {
    const ptr: T[] = malloc(n * sizeof<T>)
    return {
        beg: ptr,
        end: &ptr[n]
    }
}

function<T> vec_free(&self: vec<T>) {
    free(self.beg)
    self.beg = self.end = 0
}

function<T> vec_size(const &self: vec<T>): u64 {
    return self.end - self.beg
}

function<T> vec_front(&self: vec<T>): &T {
    return *self.beg
}

function<T> vec_back(&self: vec<T>): &T {
    return self.end[-1]
}

function<T> vec_at(&self: vec<T>, pos: u64): &T {
    return self.beg[pos]
}

function<T> vec_push(&self: vec<T>, element: T) {
    const size = vec_size<T>(self) + 1
    const ptr: T[] = realloc(self.beg, size * sizeof<T>)
    self.beg = ptr
    self.end = &ptr[size]
    vec_back<T>(self) = element
}

function<T> vec_pop(&self: vec<T>): T {
    const element: T = vec_back<T>(self)
    const size = vec_size<T>(self) - 1
    const ptr: T[] = realloc(self.beg, size * sizeof<T>)
    self.beg = ptr
    self.end = ptr + size
    return element
}

let v = vec_new<i8>(0xC)
vec_at<i8>(v, 0x0) = 'H'
vec_at<i8>(v, 0x1) = 'e'
vec_at<i8>(v, 0x2) = 'l'
vec_at<i8>(v, 0x3) = 'l'
vec_at<i8>(v, 0x4) = 'o'
vec_at<i8>(v, 0x5) = ' '
vec_at<i8>(v, 0x6) = 'W'
vec_at<i8>(v, 0x7) = 'o'
vec_at<i8>(v, 0x8) = 'r'
vec_at<i8>(v, 0x9) = 'l'
vec_at<i8>(v, 0xA) = 'd'
vec_at<i8>(v, 0xB) = '!'
vec_push<i8>(v, 0)

println(f"v = {v.beg}")
vec_free<i8>(v)

type<T> promise = {
    result: T,
    done: u1,
}

function<T> await(p: promise<T>): T {
    while (!p.done) {}
    return p.result
}

function add(a: i32, b: i32): promise<i32> {
    return {
        result: a + b,
        done: true,
    }
}

const result = await<i32>(add(123, 456))
PRINT(result)

for (let i: u64; i < 64; ++i)
    println(f"i = {2:u64 ** i}")

for (let j: u64; j < 5; ++j)
    for (let i: u64; i < 5; ++i) {
        if (i == j)
            continue
        ASSERT(i != j && "i should not be equal to j")
    }

for (let x: u64; x < 10; ++x) {
    if (x > 5)
        break
    ASSERT(x <= 5 && "x should not be greater than 5")
}

switch (0) {
    case 0 {
        break
    }
    case 1 {
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
