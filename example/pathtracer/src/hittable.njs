import aabb       from "./aabb.njs"
import hit_record from "./hit_record.njs"
import interval   from "./interval.njs"
import ray        from "./ray.njs"

type hittable = {
    hit: (const &{}.hittable, const &ray, interval, &hit_record) => u1,
    bounding_box: (const &{}.hittable) => aabb,
}.hittable

export function hit(self: hittable[const], const &r: ray, ray_t: interval, &rec: hit_record): u1 {
    if (!self)
        return false
    return self*.hit(r, ray_t, rec)
}

export function bounding_box(self: hittable[const]): aabb {
    if (!self)
        return {}
    return self*.bounding_box()
}
