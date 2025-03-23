import image    from "./image.njs"
import interval from "./interval.njs"
import vec3     from "./vec3.njs"

type color = vec3

extern function sqrt(x: f64): f64

function linear_to_gamma(component: f64): f64 {
    if (component > 0.0)
        return sqrt(component)
    return 0.0
}

export function mix(const &c1: color, const &c2: color, t: f64): color {
    return (1 - t) * c1 + t * c2
}

export function write_color(&img: image, x1: u32, x2: u32, const &{ e: [r, g, b] }: color) {
    const fr = linear_to_gamma(r)
    const fg = linear_to_gamma(g)
    const fb = linear_to_gamma(b)

    const ir: i32 = (256 * interval.intensity.clamp(fr))
    const ig: i32 = (256 * interval.intensity.clamp(fg))
    const ib: i32 = (256 * interval.intensity.clamp(fb))

    img.put(x1, x2, ir, ig, ib)
}
