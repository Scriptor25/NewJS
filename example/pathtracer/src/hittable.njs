import aabb     from "./aabb.njs"
import interval from "./interval.njs"
import ray      from "./ray.njs"
import record   from "./record.njs"

type hittable = {
    hit: (const &{}.hittable, const &ray, interval, &record) => u1,
    bounding_box: (const &{}.hittable) => aabb,
}.hittable

export function hit(self: hittable[const], const &r: ray, ray_t: interval, &rec: record): u1 {
    return self*.hit(r, ray_t, rec)
}

export function bounding_box(self: hittable[const]): aabb {
    return self*.bounding_box()
}
