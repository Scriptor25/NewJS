import ppm from "ppm.njs"

#WIDTH  "256:u32"
#HEIGHT "256:u32"

const image = ppm.begin("bin/out.ppm", WIDTH, HEIGHT)
for (let j = 0; j < HEIGHT; ++j)
    for (let i = 0; i < WIDTH; ++i) {
        const fr = (i as f32) / ((WIDTH - 1) as f32)
        const fg = (j as f32) / ((HEIGHT - 1) as f32)
        const fb = 0.2:f32
        const ir = (fr * 255.999) as i32
        const ig = (fg * 255.999) as i32
        const ib = (fb * 255.999) as i32
        ppm.write(image, ir, ig, ib)
    }
ppm.end(image)
