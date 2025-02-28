import vec2 from "./vec2.njs"
import vec3 from "./vec3.njs"
import vec4 from "./vec4.njs"
import mat4 from "./mat4.njs"

extern function println(x: i8[])

let a = vec2.new( 1.2,    3.4)
let b = vec2.new(10.0, -123.0)
let c = vec2.new(-1.5,    2.3)
let d = 6.4

println(f"{a} + {b} = {a + b}")
println(f"{a} - {b} = {a - b}")
println(f"{c} * {d} = {c * d}")
println(f"{d} * {c} = {d * c}")
println(f"{c} / {d} = {c / d}")

println(f"{vec2}")
println(f"len({a}) = {vec2.len(a)}")

let e = vec2.new(123., 456.)
println(f"e = {e}")
e *= 2.
println(f"e = {e}")
println(f"-e = {-e}")
println(f"e++ = {e++}")
println(f"--e = {--e}")

let model = mat4.identity()
model *= mat4.translation(vec3.new(1, 2, 3))
model *= mat4.scale(vec3.new(0.4, 0.5, 0.6))

println(f"model = {model}")

let v = model * vec4.new(1.0, 1.0, 1.0, 1.0)

println(f"v = {v}")
