import color       from "./color.njs"
import hit_record  from "./hit_record.njs"
import ray         from "./ray.njs"
import solid_color from "./solid_color.njs"
import texture     from "./texture.njs"

extern function malloc(count: u64): void[]

class diffuse_light {
    scatter(const &{}: diffuse_light, const &r_in: ray, const &rec: hit_record, &attenuation: color, &scattered: ray): u1 {
        return false
    },

    emitted(const &{ tex }: diffuse_light, u: f64, v: f64, const &p: point3): color {
        return texture.value(tex, u, v, p)
    },

    tex: texture[const],
}

export function create(const &emit: color): diffuse_light {
    return { tex: NEW(solid_color, { albedo: emit }) }
}
