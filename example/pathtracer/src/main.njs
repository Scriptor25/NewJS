import camera        from "./camera.njs"
import hittable_list from "./hittable_list.njs"
import sphere        from "./sphere.njs"

let sphere1 = sphere.create([ 0.0,    0.0, -1.0 ],   0.5)
let sphere2 = sphere.create([ 0.0, -100.5, -1.0 ], 100.0)

let world = hittable_list.create()
hittable_list.add(world, &sphere1)
hittable_list.add(world, &sphere2)

let cam: camera

cam.aspect_ratio = 16.0 / 9.0
cam.image_width = 400
cam.samples_per_pixel = 100

camera.render(cam, &world)
