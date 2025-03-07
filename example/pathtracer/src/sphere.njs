import interval from "./interval.njs"
import math     from "./math.njs"
import ray      from "./ray.njs"
import record   from "./record.njs"

extern function sqrt(x: f64): f64

type sphere

type sphere = {
    hit: (const &sphere, const &ray, interval, &record) => u1,
    center: point3,
    radius: f64,
    mat: material[const],
}

function hit(const &self: sphere, const &r: ray, ray_t: interval, &rec: record): u1 {
    const oc = self.center - r.origin
    const a = math.length_squared(r.direction)
    const b = math.dot(r.direction, oc)
    const c = math.length_squared(oc) - self.radius * self.radius

    const discriminant = b * b - a * c
    if (discriminant < 0)
        return false

    const sqrtd = sqrt(discriminant)

    let root = (b - sqrtd) / a
    if (!interval.surrounds(ray_t, root)) {
        root = (b + sqrtd) / a
        if (!interval.surrounds(ray_t, root))
            return false
    }

    rec.t = root
    rec.p = ray.at(r, rec.t)
    rec.mat = self.mat

    const outward_normal = (rec.p - self.center) / self.radius
    record.set_face_normal(rec, r, outward_normal)

    return true
}

export function create(const &center: point3, radius: f64, mat: material[const]): sphere {
    return {
        hit,
        center,
        radius,
        mat,
    }
}
