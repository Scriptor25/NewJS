import interval from "./interval.njs"
import math     from "./math.njs"
import ppm      from "./ppm.njs"

type color = vec3

extern function sqrt(x: f64): f64

function linear_to_gamma(component: f64): f64 {
    if (component > 0.0)
        return sqrt(component)
    return 0.0
}

function write_color(image: image&, pixel_color: color) {
    const fr = linear_to_gamma(pixel_color[0])
    const fg = linear_to_gamma(pixel_color[1])
    const fb = linear_to_gamma(pixel_color[2])

    const intensity: interval = { min: 0.000, max: 0.999 }
    const ir: i32 = (256 * interval.clamp(intensity, fr))
    const ig: i32 = (256 * interval.clamp(intensity, fg))
    const ib: i32 = (256 * interval.clamp(intensity, fb))

    ppm.write(image, ir, ig, ib)
}
