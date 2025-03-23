import color from "./color.njs"
import vec3  from "./vec3.njs"

type texture = {
    value: (const &{}.texture, f64, f64, const &point3) => color,
}.texture

export function value(self: texture[const], u: f64, v: f64, const &p: point3): color {
    return self*.value(u, v, p)
}
