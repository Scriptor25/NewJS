import math from "./math.njs"
import ppm  from "./ppm.njs"

type color = vec3

function write_color(image: image&, pixel_color: color) {
    const fr = pixel_color[0]
    const fg = pixel_color[1]
    const fb = pixel_color[2]

    const ir: i32 = (fr * 255.999)
    const ig: i32 = (fg * 255.999)
    const ib: i32 = (fb * 255.999)

    ppm.write(image, ir, ig, ib)
}
