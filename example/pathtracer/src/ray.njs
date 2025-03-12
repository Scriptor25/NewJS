import vec3 from "./vec3.njs"

class ray {

    origin: point3,
    direction: vec3,
    time: f64,

    at(const &self: ray, t: f64): point3 {
        return self.origin + t * self.direction
    },
}
