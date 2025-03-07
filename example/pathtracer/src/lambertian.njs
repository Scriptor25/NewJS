import color  from "./color.njs"
import math   from "./math.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type lambertian

type lambertian = {
    scatter: (const &lambertian, const &ray, const &record, &color, &ray) => u1,
    albedo: color,
}

function scatter(const &self: lambertian, const &r_in: ray, const &rec: record, &attenuation: color, &scattered: ray): u1 {
    let scatter_direction = rec.normal + math.random_unit_vector()

    if (math.near_zero(scatter_direction))
        scatter_direction = rec.normal

    scattered = { origin: rec.p, direction: scatter_direction }
    attenuation = self.albedo
    return true
}

export function create(const &albedo: color): lambertian {
    return {
        scatter,
        albedo,
    }
}
