import color  from "./color.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

type material = {
    scatter: (material[const], const &ray, const &record, &color, &ray) => u1
}

export function scatter(self: material[const], const &r_in: ray, const &rec: record, &attenuation: color, &scattered: ray): u1 {
    return self*.scatter(self, r_in, rec, attenuation, scattered)
}
