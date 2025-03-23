import aabb       from "./aabb.njs"
import common     from "./common.njs"
import hit_record from "./hit_record.njs"
import hittable   from "./hittable.njs"
import interval   from "./interval.njs"
import ray        from "./ray.njs"

extern function malloc(count: u64): void[]

class bvh_node {
    hit(const &{ left, right, bbox }: bvh_node, const &r: ray, ray_t: interval, &rec: hit_record): u1 {
        if (!bbox.hit(r, ray_t))
            return false
        
        const hit_left = hittable.hit(left, r, ray_t, rec)
        const hit_right = hittable.hit(right, r, { min: ray_t.min, max: hit_left ? rec.t : ray_t.max }, rec)

        return hit_left || hit_right
    },

    bounding_box(const &{ bbox }: bvh_node): aabb {
        return bbox
    },

    left: hittable[const],
    right: hittable[const],
    bbox: aabb,
}

function box_compare(a: hittable[const], b: hittable[const], axis_index: i32): i32 {
    const a_axis_interval = hittable.bounding_box(a).axis_interval(axis_index)
    const b_axis_interval = hittable.bounding_box(b).axis_interval(axis_index)
    return (a_axis_interval.min < b_axis_interval.min)
         ? (-1)
         : (a_axis_interval.min > b_axis_interval.min)
         ? (1)
         : (0)
}

function box_x_compare(a: hittable[const][], b: hittable[const][]): i32 {
    return box_compare(*a, *b, 0)
}

function box_y_compare(a: hittable[const][], b: hittable[const][]): i32 {
    return box_compare(*a, *b, 1)
}

function box_z_compare(a: hittable[const][], b: hittable[const][]): i32 {
    return box_compare(*a, *b, 2)
}

export function create(objects: hittable[const][], count: u64): bvh_node {
    let bbox = aabb.empty
    
    for (let object_index = 0; object_index < count; ++object_index)
        bbox = aabb.combine(bbox, hittable.bounding_box(objects[object_index]))

    const axis = bbox.longest_axis()

    const cmp = switch(axis) {
        default -> box_x_compare
        case 1 -> box_y_compare
        case 2 -> box_z_compare
    }
    
    let left: hittable[const]
    let right: hittable[const]

    switch (count) {
        case 1 {
            left = right = objects[0]
        }
        case 2 {
            left = objects[0]
            right = objects[1]
        }
        default {
            common.sort(objects, count, sizeof<hittable[const]>, cmp)

            const mid = count / 2:u64
            left = NEW(bvh_node, create(objects, mid))
            right = NEW(bvh_node, create(objects + mid, count - mid))
        }
    }

    return {
        left,
        right,
        bbox,
    }
}
