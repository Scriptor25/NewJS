import color  from "./color.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type material = {
    scatter: (material[], ray, record, color&, ray&) => u1
}

export function scatter(self: material[], r_in: ray, rec: record, &attenuation: color, &scattered: ray): u1 {
    return (*self).scatter(self, r_in, rec, attenuation, scattered)
}
