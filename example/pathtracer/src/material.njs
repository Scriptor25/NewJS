import color  from "./color.njs"
import ray    from "./ray.njs"
import hit_record from "./hit_record.njs"

type material = {}.material

type material = {
    scatter: (const &material, const &ray, const &hit_record, &color, &ray) => u1
}.material

export function scatter(self: material[const], const &r_in: ray, const &rec: hit_record, &attenuation: color, &scattered: ray): u1 {
    return self*.scatter(r_in, rec, attenuation, scattered)
}
