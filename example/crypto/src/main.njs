import { random } from "./random.njs"
import { create_user } from "./user.njs"

extern println(str: i8[])

let global = random()
let user1 = create_user()
let user2 = create_user()

user1.pub = user1.pvt * global
user2.pub = user2.pvt * global

user1.res = user1.pvt * user2.pub
user2.res = user2.pvt * user1.pub

println($"{user1}")
println($"{user2}")
