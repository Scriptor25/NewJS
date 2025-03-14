import vec3     from "./vec3.njs"
import ray      from "./ray.njs"

type material = {}.material

class record {
    set_face_normal(&self: record, const &r: ray, const &outward_normal: vec3) {
        self.front_face = vec3.dot(r.direction, outward_normal) < 0
        self.normal = self.front_face ? outward_normal : -outward_normal
    },

    p: point3,
    normal: vec3,
    mat: material[const],
    t: f64,
    front_face: u1,
}
