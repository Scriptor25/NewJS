import aabb      from "./aabb.njs"
import hittable  from "./hittable.njs"
import interval  from "./interval.njs"
import ray       from "./ray.njs"
import hit_record    from "./hit_record.njs"

extern function realloc(block: void[], count: u32): void[]
extern function free(block: void[])

class hittable_list {
    hit(const &{ objects, size }: hittable_list, const &r: ray, ray_t: interval, &rec: hit_record): u1 {
        let temp_rec: hit_record
        let hit_anything = false
        let closest_so_far = ray_t.max

        for (let i = 0; i < size; ++i) {
            const &object = objects[i]
            if (hittable.hit(object, r, { min: ray_t.min, max: closest_so_far }, temp_rec)) {
                hit_anything = true
                closest_so_far = temp_rec.t
                rec = temp_rec
            }
        }

        return hit_anything
    },

    add(&{ objects, size, bbox }: hittable_list, object: hittable[const]) {
        objects = realloc(objects, (size + 1) * sizeof<hittable[const]>)
        objects[size++] = object
        bbox = aabb.combine(bbox, object*.bounding_box())
    },

    objects: hittable[const][],
    size: u64,
    bbox: aabb,
}
