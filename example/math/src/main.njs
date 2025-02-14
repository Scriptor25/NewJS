import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"
import vec4 from "./vec4.njs"
import mat4 from "./mat4.njs"

extern function println(x: i8[])

let a = vec2.new( 1.2,    3.4)
let b = vec2.new(10.0, -123.0)
let c = vec2.new(-1.5,    2.3)
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

let model = mat4.new()
model *= mat4.translation(vec3.new(1, 2, 3))
model *= mat4.scale(vec3.new(0.4, 0.5, 0.6))

println($"model = {model}")

let v = model * vec4.new(1.0, 1.0, 1.0, 1.0)

println($"v = {v}")
