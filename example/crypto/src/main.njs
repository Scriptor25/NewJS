import { random_key, mix_keys, base64 } from "./key.njs"
import { create_user } from "./user.njs"
import { xor } from "./xor.njs"

extern println(str: i8[])

let global: i8[16]
random_key(global, 4u64)

let user1 = create_user()
let user2 = create_user()

mix_keys(user1.pub, user1.pvt, global, 4u64)
mix_keys(user2.pub, user2.pvt, global, 4u64)

mix_keys(user1.key, user1.pvt, user2.pub)
mix_keys(user2.key, user2.pvt, user1.pub)

let dst: i8[1024]

base64(dst, user1.key, 4u64)
println($"user 1: {dst}")

base64(dst, user2.key, 4u64)
println($"user 2: {dst}")

let src = "Hello World!"
println($"original: {src}")

xor(dst, src, user1.key, 16u64)
println($"encoded: {dst}")

xor(dst, dst, user2.key, 16u64)
println($"decoded: {dst}")
