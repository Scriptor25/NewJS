import interval from "./interval.njs"
import math     from "./math.njs"
import ppm      from "./ppm.njs"

type color = vec3

function write_color(image: image&, pixel_color: color) {
    const fr = pixel_color[0]
    const fg = pixel_color[1]
    const fb = pixel_color[2]

    const intensity: interval = { min: 0.000, max: 0.999 }
    const ir: i32 = (256 * interval.clamp(intensity, fr))
    const ig: i32 = (256 * interval.clamp(intensity, fg))
    const ib: i32 = (256 * interval.clamp(intensity, fb))

    ppm.write(image, ir, ig, ib)
}
