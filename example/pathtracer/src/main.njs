import bvh             from "./bvh.njs"
import camera          from "./camera.njs"
import checker_texture from "./checker_texture.njs"
import common          from "./common.njs"
import dielectric      from "./dielectric.njs"
import hittable_list   from "./hittable_list.njs"
import image_texture   from "./image_texture.njs"
import lambertian      from "./lambertian.njs"
import metal           from "./metal.njs"
import noise_texture   from "./noise_texture.njs"
import quad            from "./quad.njs"
import sphere          from "./sphere.njs"
import vec3            from "./vec3.njs"

extern function malloc(count: u64): void[]
extern function free(block: void[])

function bouncing_spheres() {
    const world = NEW(hittable_list, {})

    const checker = NEW(checker_texture, checker_texture.create(0.32, { e: [.2, .3, .1] }, { e: [.9, .9, .9] }))
    const ground_material = NEW(lambertian, { tex: checker })
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
                    const mat = NEW(lambertian, lambertian.solid(albedo))
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

    const material2 = NEW(lambertian, lambertian.solid({ e: [0.4, 0.2, 0.1] }))
    const sphere2 = NEW(sphere, sphere.stationary({ e: [-4, 1, 0] }, 1.0, material2))
    world*.add(sphere2)

    const material3 = NEW(metal, { albedo: { e: [0.7, 0.6, 0.5] }, fuzz: 0.0 })
    const sphere3 = NEW(sphere, sphere.stationary({ e: [4, 1, 0] }, 1.0, material3))
    world*.add(sphere3)

    const world_bvh = NEW(bvh_node, bvh.create(world*.objects, world*.size))

    let cam: camera = {}

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
}

function checker_spheres() {}

function earth_sphere() {
    const earth_texture = NEW(image_texture, image_texture.create("res/earthmap.jpg"))
    const earth_surface = NEW(lambertian, { tex: earth_texture })
    const globe = NEW(sphere, sphere.stationary({ e: [0, 0, 0] }, 2, earth_surface))

    let cam: camera = {}

    cam.aspect_ratio      = 16.0 / 9.0
    cam.image_width       = 400
    cam.samples_per_pixel = 100
    cam.max_depth         = 50

    cam.vfov     = 20
    cam.lookfrom = { e: [0, 0, 12] }
    cam.lookat   = { e: [0, 0, 0] }
    cam.vup      = { e: [0, 1, 0] }

    cam.render(globe)
}

function perlin_spheres() {
    const world = NEW(hittable_list, {})

    const tex = NEW(noise_texture, noise_texture.create(4))
    const mat = NEW(lambertian, lambertian.create(tex))

    const sphere1 = NEW(sphere, sphere.stationary({ e: [0, -1000, 0] }, 1000, mat))
    world*.add(sphere1)

    const sphere2 = NEW(sphere, sphere.stationary({ e: [0, 2, 0] }, 2, mat))
    world*.add(sphere2)

    let cam: camera = {}

    cam.aspect_ratio      = 16.0 / 9.0
    cam.image_width       = 400
    cam.samples_per_pixel = 100
    cam.max_depth         = 50

    cam.vfov     = 20
    cam.lookfrom = { e: [13, 2, 3] }
    cam.lookat   = { e: [ 0, 0, 0] }
    cam.vup      = { e: [ 0, 1, 0] }

    cam.render(world)
}

function quads() {
    const world = NEW(hittable_list, {})

    const left_red     = NEW(lambertian, lambertian.solid({ e: [1.0, 0.2, 0.2] }))
    const back_green   = NEW(lambertian, lambertian.solid({ e: [0.2, 1.0, 0.2] }))
    const right_blue   = NEW(lambertian, lambertian.solid({ e: [0.2, 0.2, 1.0] }))
    const upper_orange = NEW(lambertian, lambertian.solid({ e: [1.0, 0.5, 0.0] }))
    const lower_teal   = NEW(lambertian, lambertian.solid({ e: [0.2, 0.8, 0.8] }))

    const quad1 = NEW(quad, quad.create({ e: [-3, -2, 5] }, { e: [0, 0, -4] }, { e: [0, 4,  0] },     left_red))
    const quad2 = NEW(quad, quad.create({ e: [-2, -2, 0] }, { e: [4, 0,  0] }, { e: [0, 4,  0] },   back_green))
    const quad3 = NEW(quad, quad.create({ e: [ 3, -2, 1] }, { e: [0, 0,  4] }, { e: [0, 4,  0] },   right_blue))
    const quad4 = NEW(quad, quad.create({ e: [-2,  3, 1] }, { e: [4, 0,  0] }, { e: [0, 0,  4] }, upper_orange))
    const quad5 = NEW(quad, quad.create({ e: [-2, -3, 5] }, { e: [4, 0,  0] }, { e: [0, 0, -4] },   lower_teal))

    world*.add(quad1)
    world*.add(quad2)
    world*.add(quad3)
    world*.add(quad4)
    world*.add(quad5)

    let cam: camera = {}

    cam.aspect_ratio      = 1.0
    cam.image_width       = 400
    cam.samples_per_pixel = 100
    cam.max_depth         = 50

    cam.vfov     = 80
    cam.lookfrom = { e: [0, 0, 9] }
    cam.lookat   = { e: [0, 0, 0] }
    cam.vup      = { e: [0, 1, 0] }

    cam.render(world)
}

quads()
