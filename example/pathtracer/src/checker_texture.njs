import color       from "./color.njs"
import solid_color from "./solid_color.njs"
import texture     from "./texture.njs"
import vec3        from "./vec3.njs"

extern function floor(x: f64): f64
extern function malloc(count: u64): void[]

class checker_texture {
    value(const &{ inv_scale, even, odd }: checker_texture, u: f64, v: f64, const &p: point3): color {
        const x: i32 = floor(inv_scale * p[0])
        const y: i32 = floor(inv_scale * p[1])
        const z: i32 = floor(inv_scale * p[2])

        const is_even = (x + y + z) % 2 == 0

        return texture.value(is_even ? even : odd, u, v, p)
    },

    inv_scale: f64,
    even: texture[const],
    odd: texture[const],
}

export function create(scale: f64, const &c1: color, const &c2: color): checker_texture {
    return {
        inv_scale: 1.0 / scale,
        even: NEW(solid_color, { albedo: c1 }),
        odd: NEW(solid_color, { albedo: c2 }),
    }
}
