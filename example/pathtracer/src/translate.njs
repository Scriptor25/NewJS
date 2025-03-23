import aabb       from "./aabb.njs"
import hit_record from "./hit_record.njs"
import hittable   from "./hittable.njs"
import interval   from "./interval.njs"
import ray        from "./ray.njs"
import vec3       from "./vec3.njs"

class translate {
    hit(const &{ object, offset }: translate, const &r: ray, ray_t: interval, &rec: hit_record): u1 {
        const offset_r: ray = { origin: r.origin - offset, direction: r.direction, time: r.time }

        if (!hittable.hit(object, offset_r, ray_t, rec))
            return false
        
        rec.p += offset

        return true
    },

    bounding_box(const &{ bbox }: translate): aabb {
        return bbox
    },

    object: hittable[const],
    offset: vec3,
    bbox: aabb,
}

export function create(object: hittable[const], const &offset: vec3): translate {
    const bbox = hittable.bounding_box(object) + offset
    return { object, offset, bbox }
}
