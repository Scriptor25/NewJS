import vec3 from "./vec3.njs"

class time3 {
    at(const &{ beg, end }: time3, t: f64): vec3 {
        t = (t < 0) ? (0) : (t > 1) ? (1) : t
        return (1 - t) * beg + t * end
    },

    beg: vec3,
    end: vec3,
}
