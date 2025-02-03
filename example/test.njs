extern println(message: i8[])
extern malloc(n: u64): void[]
extern realloc(block: void[], n: u64): void[]
extern free(block: void[])

type User = {
    name: i8[],
    surname: i8[],
    age: u32,
    type: i8[],
}

let u1: User = {
    name: "Felix",
    surname: "Schreiber",
    age: 18,
    type: "App Development"
}

function printUser({ name: n, surname: s, age: a, type: t }: User&) {
    println($" - {n} {s} ({a}), {t}")
}

println("IT WORKS!")
printUser(u1)

function recurse(n: u32): u32 {
    if (n <= 1) return n
    return recurse(n - 1) + recurse(n - 2)
}

let n: u32 = 10
let r = recurse(n)
println($"recurse({n}) = {r}")

let a1: u32[3] = [1, 2, 3]
let t1 = ["Felix", u1, a1]

function printArray(array: u32[3]) {
    println($"{array}")
}

printArray(a1)

for (let i: u32; i < 3; ++i)
    println($"a1[{i}] = {a1[i]}")

function foo(): (): u32 {
    function bar(): u32 {
        return 123
    }
    return bar
}

println($"foo() = {typeof(foo())}")
println($"foo()() = {foo()()}")

let [name1, {name, surname, age, type}] = ["Felix", u1]

println($"{name1} {name} {surname} {age} {type} {[4:u32, 5:u32, 6:u32]}")
println($"t1 = {t1}")

type<T> vec = {
    beg: T[],
    end: T[],
}

template<T> vec_new(n: u64): vec<T> {
    const ptr: T[] = malloc(n * sizeof<T>)
    return {
        beg: ptr,
        end: &ptr[n],
    }
}

template<T> vec_free(self: vec<T>&) {
    free(self.beg)
    self.beg = self.end = 0
}

template<T> vec_size(self: vec<T>&): u64 {
    return self.end - self.beg
}

template<T> vec_front(self: vec<T>&): T& {
    return *self.beg
}

template<T> vec_back(self: vec<T>&): T& {
    return self.end[-1]
}

template<T> vec_at(self: vec<T>&, pos: u64): T& {
    return self.beg[pos]
}

template<T> vec_push(self: vec<T>&, element: T) {
    const size = vec_size<T>(self) + 1
    const ptr: T[] = realloc(self.beg, size * sizeof<T>)
    self.beg = ptr
    self.end = &ptr[size]
    vec_back<T>(self) = element
}

template<T> vec_pop(self: vec<T>&): T {
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
vec_push<i8>(v, '\x00')

println(v.beg)
vec_free<i8>(v)
