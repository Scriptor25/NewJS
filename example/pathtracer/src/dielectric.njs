import color  from "./color.njs"
import common from "./common.njs"
import vec3   from "./vec3.njs"
import ray    from "./ray.njs"
import hit_record from "./hit_record.njs"

extern function sqrt(x: f64): f64
extern function fmin(a: f64, b: f64): f64

function reflectance(cosine: f64, refraction_index: f64): f64 {
    const r0 = (1 - refraction_index) / (1 + refraction_index)
    const r0s = r0 * r0
    return r0s + (1 - r0s) * ((1 - cosine) ** 5)
}

class dielectric {
    scatter(const &{ albedo, refraction_index }: dielectric, const &r_in: ray, const &rec: hit_record, &attenuation: color, &scattered: ray): u1 {
        const ri = rec.front_face ? 1.0 / refraction_index : refraction_index

        const unit_direction = vec3.unit_vector(r_in.direction)
        const cos_theta = fmin(vec3.dot(-unit_direction, rec.normal), 1.0)
        const sin_theta = sqrt(1.0 - cos_theta * cos_theta)

        const cannot_refract = ri * sin_theta > 1.0
        let direction: vec3

        if (cannot_refract || reflectance(cos_theta, ri) > common.random())
            direction = vec3.reflect(unit_direction, rec.normal)
        else
            direction = vec3.refract(unit_direction, rec.normal, ri)

        scattered = { origin: rec.p, direction, time: r_in.time }
        attenuation = albedo
        return true
    },

    emitted(const &self: dielectric, u: f64, v: f64, const &p: point3): color {
        return {}
    },

    albedo: color,
    refraction_index: f64,
}
