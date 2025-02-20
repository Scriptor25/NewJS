import color  from "./color.njs"
import math   from "./math.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type metal = {
    scatter: (metal&, ray, record, color&, ray&) => u1,
    albedo: color,
    fuzz: f64,
}

function scatter(self: metal&, r_in: ray, rec: record, attenuation: color&, scattered: ray&): u1 {
    let reflected = math.reflect(r_in.direction, rec.normal)
    reflected = math.unit_vector(reflected) + (self.fuzz * math.random_unit_vector())
    scattered = { origin: rec.p, direction: reflected }
    attenuation = self.albedo
    return math.dot(scattered.direction, rec.normal) > 0
}

function create(albedo: color, fuzz: f64): metal {
    return {
        scatter,
        albedo,
        fuzz,
    }
}
