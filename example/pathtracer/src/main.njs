import camera        from "./camera.njs"
import common        from "./common.njs"
import dielectric    from "./dielectric.njs"
import hittable_list from "./hittable_list.njs"
import lambertian    from "./lambertian.njs"
import vec3          from "./vec3.njs"
import metal         from "./metal.njs"
import sphere        from "./sphere.njs"

extern function malloc(count: u64): void[]
extern function free(block: void[])

extern function println(msg: i8[])

function<T> make(const &value: T): T[] {
    const ptr: T[] = malloc(sizeof<T>);
    (*ptr) = value;
    return ptr
}

const world = make<hittable_list>(hittable_list.create())

const ground_material = make<lambertian>(lambertian.create(vec3.create(0.5, 0.5, 0.5)))
const ground_sphere = make<sphere>(sphere.create(vec3.create(0, -1000, 0), 1000, ground_material))
world*.add(ground_sphere)

for (let a = -11; a < 11; ++a) {
    for (let b = -11; b < 11; ++b) {
        const choose_mat = common.random()
        const center = vec3.create(
            a + 0.9 * common.random(),
            0.2,
            b + 0.9 * common.random(),
        )

        if ((center - vec3.create(4, 0.2, 0)).length() > 0.9) {
            let sphere_material: material[const]
            if (choose_mat < 0.8) {
                // diffuse
                const albedo = vec3.random_vector() * vec3.random_vector()
                sphere_material = make<lambertian>(lambertian.create(albedo))
            } else if (choose_mat < 0.95) {
                // metal
                const albedo = vec3.random_range_vector(0.5, 1)
                const fuzz = common.random_range(0, 0.5)
                sphere_material = make<metal>(metal.create(albedo, fuzz))
            } else {
                // glass
                sphere_material = make<dielectric>(dielectric.create(vec3.create(1, 1, 1), 1.5))
            }

            const sphere = make<sphere>(sphere.create(center, 0.2, sphere_material))
            world*.add(sphere)
        }
    }
}

const material1 = make<dielectric>(dielectric.create(vec3.create(1, 1, 1), 1.5))
const sphere1 = make<sphere>(sphere.create(vec3.create(0, 1, 0), 1.0, material1))
world*.add(sphere1)

const material2 = make<lambertian>(lambertian.create(vec3.create(0.4, 0.2, 0.1)))
const sphere2 = make<sphere>(sphere.create(vec3.create(-4, 1, 0), 1.0, material2))
world*.add(sphere2)

const material3 = make<metal>(metal.create(vec3.create(0.7, 0.6, 0.5), 0.0))
const sphere3 = make<sphere>(sphere.create(vec3.create(4, 1, 0), 1.0, material3))
world*.add(sphere3)

let cam = camera.create()

cam.aspect_ratio      = 16.0 / 9.0
cam.image_width       = 1200
cam.samples_per_pixel = 100
cam.max_depth         = 50

cam.vfov     = 20
cam.lookfrom = vec3.create(13, 2, 3)
cam.lookat   = vec3.create(0, 0, 0)
cam.vup      = vec3.create(0, 1, 0)

cam.defocus_angle = 0.6
cam.focus_dist    = 10.0

cam.render(world)
