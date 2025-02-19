import math from "./math.njs"

type ray = {
    origin: point3,
    direction: vec3,
}

function ray(origin: point3, direction: vec3): ray {
    return { origin, direction }
}

function at(self: ray&, t: f64): point3 {
    return self.origin + t * self.direction
}
