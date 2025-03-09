import color    from "./color.njs"
import common   from "./common.njs"
import hittable from "./hittable.njs"
import material from "./material.njs"
import math     from "./math.njs"
import ppm      from "./ppm.njs"
import pthread  from "./pthread.njs"
import ray      from "./ray.njs"
import record   from "./record.njs"

extern let std_in:  FILE[]
extern let std_out: FILE[]
extern let std_err: FILE[]

extern function fprintf(stream: FILE[], format: i8[], ...)
extern function fflush(stream: FILE[])

extern function tan(x: f64): f64

type camera = {
    aspect_ratio: f64,

    image_width: u32,
    image_height: u32,

    samples_per_pixel: u32,
    pixel_sample_scale: f64,

    max_depth: u32,

    vfov: f64,
    lookfrom: point3,
    lookat: point3,
    vup: vec3,

    defocus_angle: f64,
    focus_dist: f64,

    center: point3,
    pixel00_loc: point3,

    pixel_delta_u: vec3,
    pixel_delta_v: vec3,

    u: vec3,
    v: vec3,
    w: vec3,

    defocus_disk_u: vec3,
    defocus_disk_v: vec3,
}

function initialize(&self: camera) {
    self.image_height = self.image_width / self.aspect_ratio

    self.pixel_sample_scale = 1.0 / self.samples_per_pixel

    self.center = self.lookfrom

    const theta = common.to_radians(self.vfov)
    const h = tan(theta / 2)
    const viewport_height = 2 * h * self.focus_dist
    const viewport_width = viewport_height * ((self.image_width as f64) / (self.image_height as f64))

    self.w = math.unit_vector(self.lookfrom - self.lookat)
    self.u = math.unit_vector(math.cross(self.vup, self.w))
    self.v = math.cross(self.w, self.u)

    const viewport_u = viewport_width * self.u
    const viewport_v = -viewport_height * self.v

    self.pixel_delta_u = viewport_u / (self.image_width as f64)
    self.pixel_delta_v = viewport_v / (self.image_height as f64)

    const viewport_upper_left = self.center - (self.focus_dist * self.w) - viewport_u / 2.0 - viewport_v / 2.0
    self.pixel00_loc = viewport_upper_left + 0.5 * (self.pixel_delta_u + self.pixel_delta_v)

    const defocus_radius = self.focus_dist * tan(common.to_radians(self.defocus_angle / 2))
    self.defocus_disk_u = self.u * defocus_radius
    self.defocus_disk_v = self.v * defocus_radius
}

function ray_color(const &self: camera, r: ray, depth: u32, world: hittable[]): color {
    if (!depth)
        return [ 0.0, 0.0, 0.0 ]

    let rec: record

    if (hittable.hit(world, r, { min: 0.001, max: infinity }, rec)) {
        let attenuation: color
        let scattered: ray
        if (material.scatter(rec.mat, r, rec, attenuation, scattered))
            return attenuation * ray_color(self, scattered, depth - 1, world)
        return [ 0.0, 0.0, 0.0 ]
    }

    const unit_direction = math.unit_vector(r.direction)
    const a = 0.5 * (unit_direction[1] + 1.0)
    return (1.0 - a) * [ 1.0, 1.0, 1.0 ]:vec3 + a * [ 0.5, 0.7, 1.0 ]:vec3
}

function sample_square(): vec3 {
    return [ common.random() - 0.5, common.random() - 0.5, 0.0 ]
}

function defocus_disk_sample(const &self: camera): point3 {
    const p = math.random_in_unit_disk()
    return self.center + (p[0] * self.defocus_disk_u) + (p[1] * self.defocus_disk_v)
}

function get_ray(const &self: camera, i: u32, j: u32): ray {
    const offset = sample_square()
    const pixel_sample = self.pixel00_loc
                       + ((i + offset[0]) * self.pixel_delta_u)
                       + ((j + offset[1]) * self.pixel_delta_v)

    const origin = (self.defocus_angle <= 0)
                 ? self.center
                 : defocus_disk_sample(self)
    const direction = pixel_sample - origin

    return { origin, direction }
}

type line_arg_t = {
    const &self: camera,
    world: hittable[],
    &img: image_t,
    j: u32,
}

extern function srand(seed: u32): void

function render_line(args: void[]): void[] {
    const &{self, world, img, j} = *(args as line_arg_t[const])

    fprintf(std_err, "[%4d / %4d]\n", j + 1:u32, self.image_height)
    fflush(std_err)

    srand(j * 123)

    for (let i: u32; i < self.image_width; ++i) {
        let pixel_color: color
        for (let sample = 0; sample < self.samples_per_pixel; ++sample) {
            pixel_color += ray_color(self, get_ray(self, i, j), self.max_depth, world)
        }
        color.write_color(img, i, j, self.pixel_sample_scale * pixel_color)
    }

    return 0
}

type<F, S> pair = {
    fst: F,
    snd: S,
}

extern function malloc(count: u64): void[]
extern function free(block: void[]): void

export function render(&self: camera, world: hittable[]) {
    initialize(self)

    let img = ppm.create("./out.ppm", self.image_width, self.image_height)
    const ts: pair<pthread_t, line_arg_t>[] = malloc(self.image_height * sizeof<pair<pthread_t, line_arg_t> >)
    for (let j: u32; j < self.image_height; ++j) {
        ts[j].snd = {
            self,
            world,
            img,
            j,
        }
        pthread.create(ts[j].fst, 0, render_line, &ts[j].snd)
    }
    for (let j: u32; j < self.image_height; ++j)
        pthread.join(ts[j].fst, 0)
    free(ts)
    ppm.flush(img)
    ppm.close(img)

    fprintf(std_err, "\rDone         \n")
    fflush(std_err)
}
