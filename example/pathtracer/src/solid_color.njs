import color from "./color.njs"
import vec3  from "./vec3.njs"

class solid_color {
    value(const &{ albedo }: solid_color, u: f64, v: f64, const &p: point3): color {
        return albedo
    },

    albedo: color,
}
