import color  from "./color.njs"
import common from "./common.njs"
import math   from "./math.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

extern function sqrt(x: f64): f64
extern function fmin(a: f64, b: f64): f64

type dielectric

type dielectric = {
    scatter: (dielectric&, ray, record, color&, ray&) => u1,
    albedo: color,
    refraction_index: f64,
}

function reflectance(cosine: f64, refraction_index: f64): f64 {
    const r0 = (1 - refraction_index) / (1 + refraction_index)
    const r0s = r0 * r0
    return r0s + (1 - r0s) * ((1 - cosine) ** 5)
}

function scatter(&self: dielectric, r_in: ray, rec: record, &attenuation: color, &scattered: ray): u1 {
    const ri = rec.front_face ? 1.0 / self.refraction_index : self.refraction_index

    const unit_direction = math.unit_vector(r_in.direction)
    const cos_theta = fmin(math.dot(-unit_direction, rec.normal), 1.0)
    const sin_theta = sqrt(1.0 - cos_theta * cos_theta)

    const cannot_refract = ri * sin_theta > 1.0
    let direction: vec3

    if (cannot_refract || reflectance(cos_theta, ri) > common.random())
        direction = math.reflect(unit_direction, rec.normal)
    else
        direction = math.refract(unit_direction, rec.normal, ri)

    scattered = { origin: rec.p, direction }
    attenuation = self.albedo
    return true
}

export function create(albedo: color, refraction_index: f64): dielectric {
    return {
        scatter,
        albedo,
        refraction_index,
    }
}
