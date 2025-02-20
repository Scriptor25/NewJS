import camera        from "./camera.njs"
import dielectric    from "./dielectric.njs"
import hittable_list from "./hittable_list.njs"
import lambertian    from "./lambertian.njs"
import metal         from "./metal.njs"
import sphere        from "./sphere.njs"

let material_ground = lambertian.create([ 0.8, 0.8, 0.0 ])
let material_center = lambertian.create([ 0.1, 0.2, 0.5 ])
let material_left   = dielectric.create([ 1.0, 1.0, 1.0 ], 1.50 / 1.00)
let material_bubble = dielectric.create([ 1.0, 1.0, 1.0 ], 1.00 / 1.50)
let material_right  =      metal.create([ 0.8, 0.6, 0.2 ], 1.0)

let sphere1 = sphere.create([  0.0, -100.5, -1.0 ], 100.0, &material_ground);
let sphere2 = sphere.create([  0.0,    0.0, -1.2 ],   0.5, &material_center);
let sphere3 = sphere.create([ -1.0,    0.0, -1.0 ],   0.5, &material_left);
let sphere4 = sphere.create([ -1.0,    0.0, -1.0 ],   0.4, &material_bubble);
let sphere5 = sphere.create([  1.0,    0.0, -1.0 ],   0.5, &material_right);

let world = hittable_list.create()
hittable_list.add(world, &sphere1)
hittable_list.add(world, &sphere2)
hittable_list.add(world, &sphere3)
hittable_list.add(world, &sphere4)
hittable_list.add(world, &sphere5)

let cam: camera

cam.aspect_ratio = 16.0 / 9.0
cam.image_width = 400
cam.samples_per_pixel = 100
cam.max_depth = 50
cam.vfov = 20
cam.lookfrom = [ -2.0, 2.0, 1.0 ]
cam.lookat = [ 0.0, 0.0, -1.0 ]
cam.vup = [ 0.0, 1.0, 0.0 ]
cam.defocus_angle = 10.0
cam.focus_dist = 3.4

camera.render(cam, &world)
