import color       from "./color.njs"
import hit_record  from "./hit_record.njs"
import ray         from "./ray.njs"
import solid_color from "./solid_color.njs"
import texture     from "./texture.njs"
import vec3        from "./vec3.njs"

extern function malloc(count: u64): void[]

class lambertian {
    scatter(const &{ tex }: lambertian, const &r_in: ray, const &rec: hit_record, &attenuation: color, &scattered: ray): u1 {
        let direction = rec.normal + vec3.random_unit_vector()

        if (direction.near_zero())
            direction = rec.normal

        scattered = { origin: rec.p, direction, time: r_in.time }
        attenuation = texture.value(tex, rec.u, rec.v, rec.p)
        return true
    },

    emitted(const &self: lambertian, u: f64, v: f64, const &p: point3): color {
        return {}
    },

    tex: texture[const],
}

export function solid(const &albedo: color): lambertian {
    return { tex: NEW(solid_color, { albedo }) }
}

export function create(tex: texture[const]): lambertian {
    return { tex }
}
