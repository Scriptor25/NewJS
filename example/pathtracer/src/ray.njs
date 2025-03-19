import vec3 from "./vec3.njs"

class ray {
    at(const &{ origin, direction }: ray, t: f64): point3 {
        return origin + t * direction
    },

    origin: point3,
    direction: vec3,
    time: f64,
}
