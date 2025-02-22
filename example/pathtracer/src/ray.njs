import math from "./math.njs"

type ray = {
    origin: point3,
    direction: vec3,
}

export function at(const &self: ray, t: f64): point3 {
    return self.origin + t * self.direction
}
