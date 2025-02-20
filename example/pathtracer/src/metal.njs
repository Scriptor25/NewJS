import color  from "./color.njs"
import math   from "./math.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type metal = {
    scatter: (metal&, ray, record, color&, ray&) => u1,
    albedo: color,
}

function scatter(self: metal&, r_in: ray, rec: record, attenuation: color&, scattered: ray&): u1 {
    let reflected = math.reflect(r_in.direction, rec.normal)
    scattered = { origin: rec.p, direction: reflected }
    attenuation = self.albedo
    return true
}

function create(albedo: color): metal {
    return {
        scatter,
        albedo,
    }
}
