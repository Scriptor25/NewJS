import color     from "./color.njs"
import rtw_image from "./rtw_image.njs"
import vec3      from "./vec3.njs"

class image_texture {
    value(const &{ img }: image_texture, u: f64, v: f64, const &p: point3): color {
        return img.at(u, v)
    },

    img: rtw_image,
}

export function create(filename: string): image_texture {
    return { img: rtw_image.create(filename) }
}
