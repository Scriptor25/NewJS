import interval from "./interval.njs"
import ray      from "./ray.njs"
import vec3     from "./vec3.njs"

class aabb {
    axis_interval(const &{ x, y, z }: aabb, n: i64): const &interval {
        if (n == 1) return y
        if (n == 2) return z
        return x
    },

    hit(const &self: aabb, const &{ origin, direction }: ray, ray_t: interval): u1 {
        for (let axis: i64; axis < 3; ++axis) {
            const &ax = self.axis_interval(axis)
            const adinv = 1 / direction[axis]

            const t0 = (ax.min - origin[axis]) * adinv
            const t1 = (ax.max - origin[axis]) * adinv

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0
                if (t1 < ray_t.max) ray_t.max = t1
            } else {
                if (t1 > ray_t.min) ray_t.min = t1
                if (t0 < ray_t.max) ray_t.max = t0
            }

            if (ray_t.max <= ray_t.min)
                return false
        }

        return true
    },

    longest_axis(const &{ x, y, z }: aabb): i32 {
        if (x.size() > y.size())
            return x.size() > z.size() ? (0) : (2)
        return y.size() > z.size() ? (1) : (2)
    },

    x: interval,
    y: interval,
    z: interval,
}

export function points(const &a: point3, const &b: point3): aabb {
    const x = (a[0] <= b[0]) ? ({ min: a[0], max: b[0] }:interval) : ({ min: b[0], max: a[0] }:interval)
    const y = (a[1] <= b[1]) ? ({ min: a[1], max: b[1] }:interval) : ({ min: b[1], max: a[1] }:interval)
    const z = (a[2] <= b[2]) ? ({ min: a[2], max: b[2] }:interval) : ({ min: b[2], max: a[2] }:interval)
    return { x, y, z }
}

export function combine(const &a: aabb, const &b: aabb): aabb {
    const x = interval.combine(a.x, b.x)
    const y = interval.combine(a.y, b.y)
    const z = interval.combine(a.z, b.z)
    return { x, y, z }
}

export const empty: aabb = { x: interval.empty, y: interval.empty, z: interval.empty }
export const universe: aabb = { x: interval.universe, y: interval.universe, z: interval.universe }
