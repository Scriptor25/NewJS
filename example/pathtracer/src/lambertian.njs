import color  from "./color.njs"
import vec3   from "./vec3.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type lambertian = {
    scatter: (const &lambertian, const &ray, const &record, &color, &ray) => u1,

    albedo: color,
}

function scatter(const &self: lambertian, const &r_in: ray, const &rec: record, &attenuation: color, &scattered: ray): u1 {
    let direction = rec.normal + vec3.random_unit_vector()

    if (direction.near_zero())
        direction = rec.normal

    scattered = ray.create(rec.p, direction, 0)
    attenuation = self.albedo
    return true
}

export function create(const &albedo: color): lambertian {
    return {
        scatter,
        
        albedo,
    }
}
