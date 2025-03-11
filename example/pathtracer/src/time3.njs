import vec3 from "./vec3.njs"

type time3 = {
    at: (const &time3, f64) => vec3,

    beg: vec3,
    end: vec3,
}

function at(const &self: time3, t: f64): vec3 {
    t = (t < 0) ? (0) : (t > 1) ? (1) : t
    return (1 - t) * self.beg + t * self.end
}

export function create(beg: vec3, end: vec3): time3 {
    return {
        at,

        beg,
        end,
    }
}
