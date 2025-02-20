import color    from "./color.njs"
import common   from "./common.njs"
import hittable from "./hittable.njs"
import math     from "./math.njs"
import ppm      from "./ppm.njs"
import ray      from "./ray.njs"
import record   from "./record.njs"

extern let std_in:  FILE[]
extern let std_out: FILE[]
extern let std_err: FILE[]

extern function fprintf(stream: FILE[], format: i8[], ...)
extern function fflush(stream: FILE[])

type camera = {
    aspect_ratio: f64,

    image_width: u32,
    image_height: u32,

    samples_per_pixel: u32,
    pixel_sample_scale: f64,

    center: point3,
    pixel00_loc: point3,

    pixel_delta_u: vec3,
    pixel_delta_v: vec3,
}

function initialize(self: camera&) {
    self.image_height = self.image_width / self.aspect_ratio

    self.pixel_sample_scale = 1.0 / self.samples_per_pixel

    self.center = [ 0.0, 0.0, 0.0 ]

    const focal_length = 1.0
    const viewport_height = 2.0
    const viewport_width = viewport_height * ((self.image_width as f64) / (self.image_height as f64))

    const viewport_u: vec3 = [ viewport_width, 0.0, 0.0 ]
    const viewport_v: vec3 = [ 0.0, -viewport_height, 0.0 ]

    self.pixel_delta_u = viewport_u / (self.image_width as f64)
    self.pixel_delta_v = viewport_v / (self.image_height as f64)

    const viewport_upper_left = self.center - [ 0.0, 0.0, focal_length ]:vec3 - viewport_u / 2.0 - viewport_v / 2.0
    self.pixel00_loc = viewport_upper_left + 0.5 * (self.pixel_delta_u + self.pixel_delta_v)
}

function ray_color(self: camera&, r: ray, world: hittable[]): color {
    let rec: record

    if (hittable.hit(world, r, { min: 0.001, max: infinity }, rec)) {
        return 0.5 * [ rec.normal[0] + 1, rec.normal[1] + 1, rec.normal[2] + 1 ]:color
    }

    const unit_direction = math.unit_vector(r.direction)
    const a = 0.5 * (unit_direction[1] + 1.0)
    return (1.0 - a) * [ 1.0, 1.0, 1.0 ]:vec3 + a * [ 0.5, 0.7, 1.0 ]:vec3
}

function sample_square(self: camera&): vec3 {
    return [ common.random() - 0.5, common.random() - 0.5, 0.0 ]
}

function get_ray(self: camera&, i: u32, j: u32): ray {
    const offset = sample_square(self)
    const pixel_sample = self.pixel00_loc
                       + ((i + offset[0]) * self.pixel_delta_u)
                       + ((j + offset[1]) * self.pixel_delta_v)

    const origin = self.center
    const direction = pixel_sample - origin

    return { origin, direction }
}

function render(self: camera&, world: hittable[]) {
    initialize(self)

    let image = ppm.begin("./out.ppm", self.image_width, self.image_height)
    for (let j: u32; j < self.image_height; ++j) {
        fprintf(std_err, "\r[%4d / %4d]", j + 1:u32, self.image_height)
        fflush(std_err)

        for (let i: u32; i < self.image_width; ++i) {
            let pixel_color: color
            for (let sample = 0; sample < self.samples_per_pixel; ++sample) {
                pixel_color += ray_color(self, get_ray(self, i, j), world)
            }
            color.write_color(image, self.pixel_sample_scale * pixel_color)
        }
    }
    ppm.end(image)

    fprintf(std_err, "\rDone         \n")
    fflush(std_err)
}
