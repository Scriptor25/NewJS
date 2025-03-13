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

export function write_color(&img: image, x1: u32, x2: u32, const &pixel_color: color) {
    const fr = linear_to_gamma(pixel_color[0])
    const fg = linear_to_gamma(pixel_color[1])
    const fb = linear_to_gamma(pixel_color[2])

    const ir: i32 = (256 * interval.intensity.clamp(fr))
    const ig: i32 = (256 * interval.intensity.clamp(fg))
    const ib: i32 = (256 * interval.intensity.clamp(fb))

    img.put(x1, x2, ir, ig, ib)
}
