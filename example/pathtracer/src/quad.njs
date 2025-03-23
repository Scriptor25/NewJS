import aabb       from "./aabb.njs"
import hit_record from "./hit_record.njs"
import interval   from "./interval.njs"
import vec3       from "./vec3.njs"

extern function fabs(x: f64): f64

class quad {
     hit(const &self: quad, const &r: ray, ray_t: interval, &rec: hit_record): u1 {
        const denom = vec3.dot(self.normal, r.direction)

        if (fabs(denom) < 1e-8)
            return false
        
        const t = (self.D - vec3.dot(self.normal, r.origin)) / denom
        if (!ray_t.contains(t))
            return false
        
        const p = r.at(t)
        const planar_hit_point_vector = p - self.Q
        const alpha = vec3.dot(self.w, vec3.cross(planar_hit_point_vector, self.v))
        const beta = vec3.dot(self.w, vec3.cross(self.u, planar_hit_point_vector))

        if (!self.is_interior(alpha, beta, rec))
            return false

        rec.t = t
        rec.p = p
        rec.mat = self.mat
        rec.set_face_normal(r, self.normal)

        return true
    },

    bounding_box(const &{ bbox }: quad): aabb {
        return bbox
    },

    set_bounding_box(&{ Q, u, v, bbox }: quad) {
        const bbox1 = aabb.points(Q, Q + u + v)
        const bbox2 = aabb.points(Q + u, Q + v)
        bbox = aabb.combine(bbox1, bbox2)
    },

    is_interior(const &self: quad, a: f64, b: f64, &rec: hit_record): u1 {
        if (!interval.unit.contains(a) || !interval.unit.contains(b))
            return false

        rec.u = a
        rec.v = b
        return true
    },

    Q: point3,
    u: vec3,
    v: vec3,
    w: vec3,
    mat: material[const],
    bbox: aabb,
    normal: vec3,
    D: f64,
}

export function create(const &Q: point3, const &u: vec3, const &v: vec3, mat: material[const]): quad {
    const n = vec3.cross(u, v)
    const normal = vec3.unit_vector(n)
    const D = vec3.dot(normal, Q)
    const w = vec3.part_vector(n)

    let q: quad = { Q, u, v, w, mat, normal, D }
    q.set_bounding_box()
    return q
}
