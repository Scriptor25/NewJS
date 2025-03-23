import color  from "./color.njs"
import perlin from "./perlin.njs"
import vec3   from "./vec3.njs"

extern function sin(x: f64): f64

class noise_texture {
    value(const &{ noise, scale }: noise_texture, u: f64, v: f64, const &p: point3): color {
        return { e: [.5, .5, .5] }:color * (1 + sin(scale * p[2] + 10 * noise.turb(p, 7)))
    },

    noise: perlin,
    scale: f64,
}

export function create(scale: f64): noise_texture {
    return { noise: perlin.create(), scale }
}
