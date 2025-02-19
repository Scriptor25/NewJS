import hittable  from "./hittable.njs"
import ray       from "./ray.njs"
import record    from "./record.njs"

extern function realloc(block: void[], count: u32): void[]
extern function free(block: void[])

type hittable_list = {
    hit: (hittable[], ray, f64, f64, record&) => u1,
    objects: hittable[][],
    size: u64,
}

function hit(self: hittable_list&, r: ray, ray_t_min: f64, ray_t_max: f64, rec: record&): u1 {
    let temp_rec: record
    let hit_anything = false
    let closest_so_far = ray_t_max

    for (let i = 0; i < self.size; ++i) {
        const object = self.objects[i]
        if (hittable.hit(object, r, ray_t_min, closest_so_far, temp_rec)) {
            hit_anything = true
            closest_so_far = temp_rec.t
            rec = temp_rec
        }
    }

    return hit_anything
}

function create(): hittable_list {
    return {
        hit,
        objects: 0,
        size: 0,
    }
}

function add(self: hittable_list&, object: hittable[]) {
    self.objects = realloc(self.objects, (self.size + 1) * sizeof<hittable[]>)
    self.objects[self.size++] = object
}

function destroy(self: hittable_list&) {
    free(self.objects)
    self.objects = 0
    self.size = 0
}
