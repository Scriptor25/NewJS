import aabb       from "./aabb.njs"
import common     from "./common.njs"
import hit_record from "./hit_record.njs"
import hittable   from "./hittable.njs"
import interval   from "./interval.njs"
import ray        from "./ray.njs"
import vec3       from "./vec3.njs"

extern function sin(x: f64): f64
extern function cos(x: f64): f64
extern function fmin(a: f64, b: f64): f64
extern function fmax(a: f64, b: f64): f64

class rotate_y {
    hit(const &{ object, sin_theta, cos_theta }: rotate_y, const &r: ray, ray_t: interval, &rec: hit_record): u1 {
        const origin: point3 = { e: [
            (cos_theta * r.origin[0]) - (sin_theta * r.origin[2]),
            r.origin[1],
            (sin_theta * r.origin[0]) + (cos_theta * r.origin[2]),
        ] }

        const direction: point3 = { e: [
            (cos_theta * r.direction[0]) - (sin_theta * r.direction[2]),
            r.direction[1],
            (sin_theta * r.direction[0]) + (cos_theta * r.direction[2]),
        ] }

        const rotated_r: ray = { origin, direction, time: r.time }

        if (!hittable.hit(object, rotated_r, ray_t, rec))
            return false
        
        rec.p = { e: [
            (cos_theta * rec.p[0]) + (sin_theta * rec.p[2]),
            rec.p[1],
            (-sin_theta * rec.p[0]) + (cos_theta * rec.p[2]),
        ] }
        
        rec.normal = { e: [
            (cos_theta * rec.normal[0]) + (sin_theta * rec.normal[2]),
            rec.normal[1],
            (-sin_theta * rec.normal[0]) + (cos_theta * rec.normal[2]),
        ] }

        return true
    },

    bounding_box(const &{ bbox }: rotate_y): aabb {
        return bbox
    },

    object: hittable[const],
    sin_theta: f64,
    cos_theta: f64,
    bbox: aabb,
}

export function create(object: hittable[const], angle: f64): rotate_y {
    const theta = common.to_radians(angle)
    const sin_theta = sin(theta)
    const cos_theta = cos(theta)

    let bbox = hittable.bounding_box(object)
    let min: point3 = { e: [ infinity,  infinity,  infinity] }
    let max: point3 = { e: [-infinity, -infinity, -infinity] }

    for (let i = 0; i < 2; ++i)
        for (let j = 0; j < 2; ++j)
            for (let k = 0; k < 2; ++k) {
                const x = i * bbox.x.max + (1 - i) * bbox.x.min
                const y = j * bbox.y.max + (1 - j) * bbox.y.min
                const z = k * bbox.z.max + (1 - k) * bbox.z.min

                const newx =  cos_theta * x + sin_theta * z
                const newz = -sin_theta * x + cos_theta * z

                const tester: vec3 = { e: [newx, y, newz] }

                for (let c = 0; c < 3; ++c) {
                    min[c] = fmin(min[c], tester[c])
                    max[c] = fmax(max[c], tester[c])
                }
            }
    
    bbox = aabb.points(min, max)

    return {object, sin_theta, cos_theta, bbox }
}
