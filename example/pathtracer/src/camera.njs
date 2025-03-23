import color    from "./color.njs"
import common   from "./common.njs"
import hittable from "./hittable.njs"
import image    from "./image.njs"
import interval from "./interval.njs"
import material from "./material.njs"
import pthread  from "./pthread.njs"
import ray      from "./ray.njs"
import hit_record   from "./hit_record.njs"
import vec3     from "./vec3.njs"

extern let std_in:  FILE[]
extern let std_out: FILE[]
extern let std_err: FILE[]

extern function fprintf(stream: FILE[], format: i8[], ...)
extern function fflush(stream: FILE[])
extern function srand(seed: u32): void
extern function tan(x: f64): f64
extern function malloc(count: u64): void[]
extern function free(block: void[]): void

type camera = {}.camera

type scanline_t = {
    const &self: camera,
    world: hittable[const],
    &img: image,
    j: u32,
}

class camera {

    initialize(&{ * }: camera) {
        image_height = image_width / aspect_ratio
        image_height = MAX(image_height, 1)
        focus_dist = MAX(focus_dist, 1.0)

        pixel_sample_scale = 1.0 / samples_per_pixel

        center = lookfrom

        const theta = common.to_radians(vfov)
        const h = tan(theta / 2)
        const viewport_height = 2 * h * focus_dist
        const viewport_width = viewport_height * ((image_width as f64) / (image_height as f64))

        w = vec3.unit_vector(lookfrom - lookat)
        u = vec3.unit_vector(vec3.cross(vup, w))
        v = vec3.cross(w, u)

        const viewport_u = viewport_width * u
        const viewport_v = -viewport_height * v

        pixel_delta_u = viewport_u / (image_width as f64)
        pixel_delta_v = viewport_v / (image_height as f64)

        const viewport_upper_left = center - (focus_dist * w) - viewport_u / 2.0 - viewport_v / 2.0
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v)

        const defocus_radius = focus_dist * tan(common.to_radians(defocus_angle / 2))
        defocus_disk_u = u * defocus_radius
        defocus_disk_v = v * defocus_radius
    },

    ray_color(const &self: camera, r: ray, depth: u32, world: hittable[const]): color {
        if (!depth)
            return {}

        let rec: hit_record

        if (hittable.hit(world, r, { min: 0.001, max: infinity }, rec)) {
            let attenuation: color
            let scattered: ray
            if (material.scatter(rec.mat, r, rec, attenuation, scattered))
                return attenuation * self.ray_color(scattered, depth - 1, world)
            return {}
        }

        const unit_direction = vec3.unit_vector(r.direction)
        const a = 0.5 * (unit_direction[1] + 1)
        return (1 - a) * { e: [1, 1, 1] } + a * { e: [0.5, 0.7, 1] }
    },

    sample_square(): vec3 {
        return { e: [common.random() - 0.5, common.random() - 0.5, 0] }
    },

    defocus_disk_sample(const &{ center, defocus_disk_u, defocus_disk_v }: camera): point3 {
        const p = vec3.random_in_unit_disk()
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v)
    },

    get_ray(const &self: camera, i: u32, j: u32): ray {
        const offset = self.sample_square()
        const pixel_sample = self.pixel00_loc
                           + ((i + offset[0]) * self.pixel_delta_u)
                           + ((j + offset[1]) * self.pixel_delta_v)

        const origin = (self.defocus_angle <= 0)
                     ? self.center
                     : self.defocus_disk_sample()
        const direction = pixel_sample - origin
    	const time = common.random()

        return { origin, direction, time }
    },

    render_scanline(const &self: camera, world: hittable[const], &img: image, j: u32) {
        fprintf(std_err, "[%4d / %4d]\n", j + 1:u32, self.image_height)
        fflush(std_err)

        srand(j * self.image_width)

        for (let i: u32; i < self.image_width; ++i) {
            let pixel_color: color
            for (let sample = 0; sample < self.samples_per_pixel; ++sample)
                pixel_color += self.ray_color(self.get_ray(i, j), self.max_depth, world)
            color.write_color(img, i, j, self.pixel_sample_scale * pixel_color)
        }
    },

    render(&self: camera, world: hittable[const]) {
        self.initialize()

        let img: image
        img.open("./out.ppm", self.image_width, self.image_height)

        const THREAD_COUNT: u32 = 10
        const ts: PAIR(pthread_t, scanline_t)[] = malloc(THREAD_COUNT * sizeof<PAIR(pthread_t, scanline_t)>)
        for (let j: u32; j < self.image_height; j += THREAD_COUNT) {
            for (let x: u32; x < THREAD_COUNT && j + x < self.image_height; ++x) {
                ts[x].snd = {
                    self,
                    world,
                    img,
                    j: j + x,
                }
                pthread.create(ts[x].fst, 0, $(ap: void[]): void[] {
                    const &{self, world, img, j} = *(ap as scanline_t[const])
                    self.render_scanline(world, img, j)
                    return 0
                }, &ts[x].snd)
            }
            for (let x: u32; x < THREAD_COUNT && j + x < self.image_height; ++x)
                pthread.join(ts[x].fst, 0)
            img.flush()
        }
        free(ts)

        img.flush()
        img.close()

        fprintf(std_err, "Done\n")
        fflush(std_err)
    },

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
