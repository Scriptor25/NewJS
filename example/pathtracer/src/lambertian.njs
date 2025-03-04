import color  from "./color.njs"
import math   from "./math.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type lambertian

type lambertian = {
    scatter: (&lambertian, ray, record, &color, &ray) => u1,
    albedo: color,
}

function scatter(&self: lambertian, r_in: ray, rec: record, &attenuation: color, &scattered: ray): u1 {
    let scatter_direction = rec.normal + math.random_unit_vector()

    if (math.near_zero(scatter_direction))
        scatter_direction = rec.normal

    scattered = { origin: rec.p, direction: scatter_direction }
    attenuation = self.albedo
    return true
}

export function create(albedo: color): lambertian {
    return {
        scatter,
        albedo,
    }
}
