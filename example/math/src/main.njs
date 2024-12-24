import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"
import mat4 from "./mat4.njs"

extern println(x: i8[])

let a = vec2.new( 1.2f64,   3.4f64)
let b = vec2.new(10.0f64, -123.f64)
let c = vec2.new(-1.5f64,   2.3f64)
let d = 6.4f64

println($"{a} + {b} = {a + b}")
println($"{a} - {b} = {a - b}")
println($"{c} * {d} = {c * d}")
println($"{d} * {c} = {d * c}")
println($"{c} / {d} = {c / d}")

println($"{vec2}")
println($"len({a}) = {vec2.len(a)}")

let e = vec2.new(123.f64, 456.f64)
println($"e = {e}")
e *= 2.f64
println($"e = {e}")

let m = mat4.new()


