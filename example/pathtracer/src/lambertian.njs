import color  from "./color.njs"
import vec3   from "./vec3.njs"
import ray    from "./ray.njs"
import record from "./record.njs"

class lambertian {
    scatter(const &{ albedo }: lambertian, const &r_in: ray, const &rec: record, &attenuation: color, &scattered: ray): u1 {
        let direction = rec.normal + vec3.random_unit_vector()

        if (direction.near_zero())
            direction = rec.normal

        scattered = { origin: rec.p, direction, time: r_in.time }
        attenuation = albedo
        return true
    },

    albedo: color,
}
