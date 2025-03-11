import color  from "./color.njs"
import vec3   from "./vec3.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type metal = {
    scatter: (const &metal, const &ray, const &record, &color, &ray) => u1,
    albedo: color,
    fuzz: f64,
}

function scatter(const &self: metal, const &r_in: ray, const &rec: record, &attenuation: color, &scattered: ray): u1 {
    let reflected = vec3.reflect(r_in.direction, rec.normal)
    reflected = vec3.unit_vector(reflected) + (self.fuzz * vec3.random_unit_vector())
    scattered = { origin: rec.p, direction: reflected }
    attenuation = self.albedo
    return vec3.dot(scattered.direction, rec.normal) > 0
}

export function create(const &albedo: color, fuzz: f64): metal {
    return {
        scatter,
        albedo,
        fuzz,
    }
}
