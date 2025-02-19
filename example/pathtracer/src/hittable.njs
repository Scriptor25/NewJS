import ray    from "./ray.njs"
import record from "./record.njs"

type hittable = {
    hit: (hittable[], ray, f64, f64, record&) => u1
}

function hit(self: hittable[], r: ray, ray_t_min: f64, ray_t_max: f64, rec: record&): u1 {
    return (*self).hit(self, r, ray_t_min, ray_t_max, rec)
}
