import color  from "./color.njs"
import vec3   from "./vec3.njs"
import ray    from "./ray.njs"
import hit_record from "./hit_record.njs"

class metal {
    scatter(const &{ albedo, fuzz }: metal, const &r_in: ray, const &rec: hit_record, &attenuation: color, &scattered: ray): u1 {
        let reflected = vec3.reflect(r_in.direction, rec.normal)
        reflected = vec3.unit_vector(reflected) + (fuzz * vec3.random_unit_vector())
        scattered = { origin: rec.p, direction: reflected, time: r_in.time }
        attenuation = albedo
        return vec3.dot(scattered.direction, rec.normal) > 0
    },

    albedo: color,
    fuzz: f64,
}
