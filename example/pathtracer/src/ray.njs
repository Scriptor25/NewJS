import vec3 from "./vec3.njs"

class ray {
    at(const &self: ray, t: f64): point3 {
        return self.origin + t * self.direction
    },

    origin: point3,
    direction: vec3,
    time: f64,
}
