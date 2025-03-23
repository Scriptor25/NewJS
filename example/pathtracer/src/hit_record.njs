import ray  from "./ray.njs"
import vec3 from "./vec3.njs"

type material = {}.material

class hit_record {
    set_face_normal(&{ front_face, normal }: hit_record, const &r: ray, const &outward_normal: vec3) {
        front_face = vec3.dot(r.direction, outward_normal) < 0
        normal = front_face ? outward_normal : -outward_normal
    },

    p: point3,
    normal: vec3,
    mat: material[const],
    t: f64,
    u: f64,
    v: f64,
    front_face: u1,
}
