import math     from "./math.njs"
import ray      from "./ray.njs"

type material

type record = {
    p: point3,
    normal: vec3,
    mat: material[],
    t: f64,
    front_face: u1,
}

export function set_face_normal(&self: record, r: ray, outward_normal: vec3) {
    self.front_face = math.dot(r.direction, outward_normal) < 0
    self.normal = self.front_face ? outward_normal : -outward_normal
}
