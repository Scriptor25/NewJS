import vec3 from "./vec3.njs"

type ray = {
    at: (const &ray, f64) => point3,

    origin: point3,
    direction: vec3,
	time: f64,
}

function at(const &self: ray, t: f64): point3 {
    return self.origin + t * self.direction
}

export function create(const &origin: point3, const &direction: vec3, time: f64): ray {
    return {
        at,

        origin,
        direction,
        time,
    }
}

export function empty(): ray {
    return {
        at,
    }
}
