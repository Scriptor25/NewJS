import color         from "./color.njs"
import common        from "./common.njs"
import hittable      from "./hittable.njs"
import hittable_list from "./hittable_list.njs"
import math          from "./math.njs"
import ppm           from "./ppm.njs"
import ray           from "./ray.njs"
import record        from "./record.njs"
import sphere        from "./sphere.njs"

extern let std_in:  FILE[]
extern let std_out: FILE[]
extern let std_err: FILE[]

extern function fprintf(stream: FILE[], format: i8[], ...)
extern function fflush(stream: FILE[])

extern function sqrt(x: f64): f64
extern function println(message: i8[])

function ray_color(r: ray, world: hittable[]): color {
    let rec: record
    if (hittable.hit(world, r, 0, infinity, rec)) {
        return 0.5 * [ rec.normal[0] + 1, rec.normal[1] + 1, rec.normal[2] + 1 ]:color
    }

    const unit_direction = math.unit_vector(r.direction)
    const a = 0.5 * (unit_direction[1] + 1.0)
    return (1.0 - a) * [ 1.0, 1.0, 1.0 ]:vec3 + a * [ 0.5, 0.7, 1.0 ]:vec3
}

const aspect_ratio: f64 = 16.0 / 9.0
const image_width = 400:u32
const image_height: u32 = image_width / aspect_ratio

let image = ppm.begin("./bin/out.ppm", image_width, image_height)

let world = hittable_list.create()
let sphere1 = sphere.create([ 0.0,    0.0, -1.0 ],   0.5)
hittable_list.add(world, &sphere1)
let sphere2 = sphere.create([ 0.0, -100.5, -1.0 ], 100.0)
hittable_list.add(world, &sphere2)

println($"{sphere1} {sphere2} {world}")

const focal_length = 1.0
const viewport_height = 2.0
const viewport_width = viewport_height * ((image_width as f64) / (image_height as f64))
const camera_center: point3 = [ 0.0, 0.0, 0.0 ]

const viewport_u: vec3 = [ viewport_width, 0.0, 0.0 ]
const viewport_v: vec3 = [ 0.0, -viewport_height, 0.0 ]

const pixel_delta_u = viewport_u / (image_width as f64)
const pixel_delta_v = viewport_v / (image_height as f64)

const viewport_upper_left = camera_center - [ 0.0, 0.0, focal_length ]:vec3 - viewport_u / 2.0 - viewport_v / 2.0
const pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v)

for (let j: u32; j < image.height; ++j) {
    fprintf(std_err, "\r[%4d / %4d]", j + 1:u32, image.height)
    fflush(std_err)

    for (let i: u32; i < image.width; ++i) {
        const pixel_center = pixel00_loc + ((i as f64) * pixel_delta_u) + ((j as f64) * pixel_delta_v)
        const ray_direction = pixel_center - camera_center
        const pixel_color = ray_color({ origin: camera_center, direction: ray_direction }, &world)
        color.write_color(image, pixel_color)
    }
}
ppm.end(image)

fprintf(std_err, "\rDone         \n")
fflush(std_err)
