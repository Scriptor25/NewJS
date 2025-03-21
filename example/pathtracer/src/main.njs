import bvh           from "./bvh.njs"
import camera        from "./camera.njs"
import common        from "./common.njs"
import dielectric    from "./dielectric.njs"
import hittable_list from "./hittable_list.njs"
import lambertian    from "./lambertian.njs"
import metal         from "./metal.njs"
import sphere        from "./sphere.njs"
import vec3          from "./vec3.njs"

extern function malloc(count: u64): void[]
extern function free(block: void[])

extern function println(msg: i8[])

const world = NEW(hittable_list, {})

const ground_material = NEW(lambertian, { albedo: { e: [0.5, 0.5, 0.5] } })
const ground_sphere = NEW(sphere, sphere.stationary({ e: [0, -1000, 0] }, 1000, ground_material))
world*.add(ground_sphere)

for (let a = -11; a < 11; ++a) {
    for (let b = -11; b < 11; ++b) {
        const choose_mat = common.random()
        const center = { e: [
            a + 0.9 * common.random(),
            0.2,
            b + 0.9 * common.random(),
        ] }:point3

        if ((center - { e: [4, 0.2, 0] }:point3).length() > 0.9) {
            let sp: sphere

            if (choose_mat < 0.8) {
                const albedo = vec3.random_vector() * vec3.random_vector()
                const mat = NEW(lambertian, { albedo })
                const center2 = center + { e: [0, common.random_range(0, 0.5), 0] }:vec3
                sp = sphere.moving(center, center2, 0.2, mat)
            } else if (choose_mat < 0.95) {
                const albedo = vec3.random_range_vector(0.5, 1)
                const fuzz = common.random_range(0, 0.5)
                const mat = NEW(metal, { albedo, fuzz })
                sp = sphere.stationary(center, 0.2, mat)
            } else {
                const mat = NEW(dielectric, { albedo: { e: [1, 1, 1] }, refraction_index: 1.5 })
                sp = sphere.stationary(center, 0.2, mat)
            }

            const sp_ptr = NEW(sphere, sp)
            world*.add(sp_ptr)
        }
    }
}

const material1 = NEW(dielectric, { albedo: { e: [1, 1, 1] }, refraction_index: 1.5 })
const sphere1 = NEW(sphere, sphere.stationary({ e: [0, 1, 0] }, 1.0, material1))
world*.add(sphere1)

const material2 = NEW(lambertian, { albedo: { e: [0.4, 0.2, 0.1] } })
const sphere2 = NEW(sphere, sphere.stationary({ e: [-4, 1, 0] }, 1.0, material2))
world*.add(sphere2)

const material3 = NEW(metal, { albedo: { e: [0.7, 0.6, 0.5] }, fuzz: 0.0 })
const sphere3 = NEW(sphere, sphere.stationary({ e: [4, 1, 0] }, 1.0, material3))
world*.add(sphere3)

const world_bvh = NEW(bvh_node, bvh.create(world*.objects, world*.size))

let cam: camera

cam.aspect_ratio      = 16.0 / 9.0
cam.image_width       = 400
cam.samples_per_pixel = 100
cam.max_depth         = 50

cam.vfov     = 20
cam.lookfrom = { e: [13, 2, 3] }
cam.lookat   = { e: [ 0, 0, 0] }
cam.vup      = { e: [ 0, 1, 0] }

cam.defocus_angle =  0.6
cam.focus_dist    = 10.0

cam.render(world_bvh)
