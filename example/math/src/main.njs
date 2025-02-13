import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"
import mat4 from "./mat4.njs"

extern function println(x: i8[])

let a = vec2.new( 1.2,   3.4)
let b = vec2.new(10.0, -123.)
let c = vec2.new(-1.5,   2.3)
let d = 6.4

println($"{a} + {b} = {a + b}")
println($"{a} - {b} = {a - b}")
println($"{c} * {d} = {c * d}")
println($"{d} * {c} = {d * c}")
println($"{c} / {d} = {c / d}")

println($"{vec2}")
println($"len({a}) = {vec2.len(a)}")

let e = vec2.new(123., 456.)
println($"e = {e}")
e *= 2.
println($"e = {e}")

let m = mat4.new()
m *= mat4.translation(vec3.new(1, 2, 3))
m *= mat4.scale(vec3.new(0.4, 0.5, 0.6))

println($"m = {m}")
