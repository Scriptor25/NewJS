import interval from "./interval.njs"
import ray      from "./ray.njs"
import vec3     from "./vec3.njs"

class aabb {
    pad_to_minimums(&{ x, y, z }: aabb) {
        const delta = 0.0001
        if (x.size() < delta) x = x.expand(delta)
        if (y.size() < delta) y = y.expand(delta)
        if (z.size() < delta) z = z.expand(delta)
    },

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

export function create(const &x: interval, const &y: interval, const &z: interval): aabb {
    let bbox: aabb = { x, y, z }
    bbox.pad_to_minimums()
    return bbox
}

export function points(const &{ e: [ax, ay, az] }: point3, const &{ e: [bx, by, bz] }: point3): aabb {
    const x = (ax <= bx) ? { min: ax, max: bx }:interval : { min: bx, max: ax }:interval
    const y = (ay <= by) ? { min: ay, max: by }:interval : { min: by, max: ay }:interval
    const z = (az <= bz) ? { min: az, max: bz }:interval : { min: bz, max: az }:interval
    return create(x, y, z)
}

export function combine(const &{ x: ax, y: ay, z: az }: aabb, const &{ x: bx, y: by, z: bz }: aabb): aabb {
    const x = interval.combine(ax, bx)
    const y = interval.combine(ay, by)
    const z = interval.combine(az, bz)
    return create(x, y, z)
}

export function operator+(const &{ x, y, z }: aabb, const &{ e: [ox, oy, oz] }: vec3): aabb {
    return create(x + ox, y + oy, z + oz)
}

export function operator+(const &offset: vec3, const &bbox: aabb): aabb {
    return bbox + offset
}

export const empty: aabb = create(interval.empty, interval.empty, interval.empty)
export const universe: aabb = create(interval.universe, interval.universe, interval.universe)
