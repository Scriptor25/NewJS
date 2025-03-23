import aabb     from "./aabb.njs"
import interval from "./interval.njs"
import ray      from "./ray.njs"
import hit_record   from "./hit_record.njs"
import time3    from "./time3.njs"
import vec3     from "./vec3.njs"

extern function sqrt(x: f64): f64
extern function acos(x: f64): f64
extern function atan2(y: f64, x: f64): f64

export function get_uv(const &{ e: [x, y, z] }: point3, &u: f64, &v: f64) {
    const theta = acos(-y)
    const phi = atan2(-z, x) + pi

    u = phi / (2 * pi)
    v = theta / pi
}

class sphere {
    hit(const &{ center, radius, mat }: sphere, const &r: ray, ray_t: interval, &rec: hit_record): u1 {
        const current_center = center.at(r.time)
    	const oc = current_center - r.origin
        const a = r.direction.length_squared()
        const b = vec3.dot(r.direction, oc)
        const c = oc.length_squared() - radius * radius

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
        rec.p = r.at(rec.t)
        rec.mat = mat

        const outward_normal = (rec.p - current_center) / radius
        rec.set_face_normal(r, outward_normal)

        get_uv(outward_normal, rec.u, rec.v)

        return true
    },

    bounding_box(const &{ bbox }: sphere): aabb {
        return bbox
    },

    center: time3,
    radius: f64,
    mat: material[const],
    bbox: aabb,
}

export function stationary(const &center: point3, radius: f64, mat: material[const]): sphere {
    const rvec = { e: [radius, radius, radius] }:vec3
    return {
        center: { beg: center, end: center },
        radius,
        mat,
        bbox: aabb.points(center - rvec, center + rvec),
    }
}

export function moving(const &center1: point3, const &center2: point3, radius: f64, mat: material[const]): sphere {
    const rvec = { e: [radius, radius, radius] }:vec3
    const box1 = aabb.points(center1 - rvec, center1 + rvec)
    const box2 = aabb.points(center2 - rvec, center2 + rvec)
    return {
        center: { beg: center1, end: center2 },
        radius,
        mat,
        bbox: aabb.combine(box1, box2),
    }
}
