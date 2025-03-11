import interval from "./interval.njs"
import ray      from "./ray.njs"
import record   from "./record.njs"
import time3    from "./time3.njs"
import vec3     from "./vec3.njs"

extern function sqrt(x: f64): f64

type sphere = {
    hit: (const &sphere, const &ray, interval, &record) => u1,

    center: time3,
    radius: f64,
    mat: material[const],
}

function hit(const &self: sphere, const &r: ray, ray_t: interval, &rec: record): u1 {
    const center = self.center.at(r.time)
	const oc = center - r.origin
    const a = r.direction.length_squared()
    const b = vec3.dot(r.direction, oc)
    const c = oc.length_squared() - self.radius * self.radius

    const discriminant = b * b - a * c
    if (discriminant < 0)
        return false

    const sqrtd = sqrt(discriminant)

    let root = (b - sqrtd) / a
    if (!ray_t.surrounds(root)) {
        root = (b + sqrtd) / a
        if (!ray_t.surrounds(root))
            return false
    }

    rec.t = root
    rec.p = ray.at(r, rec.t)
    rec.mat = self.mat

    const outward_normal = (rec.p - center) / self.radius
    rec.set_face_normal(r, outward_normal)

    return true
}

export function create(const &center: point3, radius: f64, mat: material[const]): sphere {
    return {
        hit,

        center: time3.create(center, center),
        radius,
        mat,
    }
}

export function moving(const &center1: point3, const &center2: point3, radius: f64, mat: material[const]): sphere {
	return {
		hit,

		center: time3.create(center1, center2),
		radius,
		mat,
	}
}
