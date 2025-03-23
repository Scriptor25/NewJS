import color      from "./color.njs"
import hit_record from "./hit_record.njs"
import ray        from "./ray.njs"

type material = {}.material

type material = {
    scatter: (const &material, const &ray, const &hit_record, &color, &ray) => u1,
    emitted: (const &material, f64, f64, const &point3) => color,
}.material

export function scatter(self: material[const], const &r_in: ray, const &rec: hit_record, &attenuation: color, &scattered: ray): u1 {
    return self*.scatter(r_in, rec, attenuation, scattered)
}

export function emitted(self: material[const], u: f64, v: f64, const &p: point3): color {
    return self*.emitted(u, v, p)
}
