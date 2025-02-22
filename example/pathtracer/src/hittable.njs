import interval from "./interval.njs"
import ray      from "./ray.njs"
import record   from "./record.njs"

type hittable

type hittable = {
    hit: (hittable[], ray, interval, record&) => u1
}

export function hit(self: hittable[], r: ray, ray_t: interval, &rec: record): u1 {
    return (*self).hit(self, r, ray_t, rec)
}
