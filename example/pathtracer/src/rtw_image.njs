import color from "./color.njs"

extern function floor(x: f64): f64
extern function ceil(x: f64): f64

extern function stbi_loadf(filename: string, x: i32[], y: i32[], channels_in_file: i32[], desired_channels: i32): f32[]
extern function stbi_image_free(retval_from_stbi_load: void[])
extern function stbi_set_flip_vertically_on_load(flag_true_if_should_flip: i32)

function ftob(x: f32): u8 {
    if (x <= 0) return 0
    if (x >= 1) return 255
    return 256 * x
}

class rtw_image {
    open(&{ fdata, width, height }: rtw_image, filename: string) {
        stbi_set_flip_vertically_on_load(true)
        fdata = stbi_loadf(filename, &width, &height, 0, 3)
    },

    close(&{ fdata, width, height }: rtw_image) {
        stbi_image_free(fdata)
        fdata = 0
        width = 0
        height = 0
    },

    pixel(const &{ fdata, width }: rtw_image, x: i32, y: i32): color {
        const index = (x + y * width) * 3
        return { e: [
            fdata[index + 0],
            fdata[index + 1],
            fdata[index + 2]
        ] }
    },

    at(const &self: rtw_image, u: f64, v: f64): color {
        if (!self.fdata)
            return { e: [1, 0, 1] }

        const ui = u * (self.width - 1)
        const vi = v * (self.height - 1)
        
        const x1 = (floor(ui) as i32) % self.width
        const x2 = (ceil(ui) as i32) % self.width
        const y1 = (floor(vi) as i32) % self.height
        const y2 = (ceil(vi) as i32) % self.height

        const p11 = self.pixel(x1, y1)
        const p12 = self.pixel(x1, y2)
        const p21 = self.pixel(x2, y1)
        const p22 = self.pixel(x2, y2)

        const c1 = color.mix(p11, p12, vi - y1)
        const c2 = color.mix(p21, p22, vi - y1)

        return color.mix(c1, c2, ui - x1)
    },

    fdata: f32[],
    width: i32,
    height: i32,
}

export function create(filename: string): rtw_image {
    let img: rtw_image = {}
    img.open(filename)
    return img
}
