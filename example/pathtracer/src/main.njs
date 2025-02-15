import ppm from "ppm.njs"

extern let std_in:  FILE[]
extern let std_out: FILE[]
extern let std_err: FILE[]

extern function fprintf(stream: FILE[], format: i8[], ...)
extern function fflush(stream: FILE[])

let image = ppm.begin("bin/out.ppm", 100, 100)
for (let j: u32; j < image.height; ++j) {
    fprintf(std_err, "\r[%4d / %4d]", j + 1:u32, image.height)
    fflush(std_err)

    for (let i: u32; i < image.width; ++i) {
        const fr = (i as f32) / ((image.width - 1) as f32)
        const fg = (j as f32) / ((image.height - 1) as f32)
        const fb = 0.2:f32

        const ir: i32 = (fr * 255.999)
        const ig: i32 = (fg * 255.999)
        const ib: i32 = (fb * 255.999)

        ppm.write(image, ir, ig, ib)
    }
}
ppm.end(image)
