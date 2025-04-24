import { random, discrete_log, base64 } from "./key.njs"
import { } from "./user.njs"
import { xor } from "./xor.njs"

extern function println(x: string)

const g = random()
const p = 18446744073709551557:u64

println(`g = ${g}, p = ${p}`)

let user1: user_t = { pvt: random() }
let user2: user_t = { pvt: random() }

user1.pub = discrete_log(g, p, user1.pvt)
user2.pub = discrete_log(g, p, user2.pvt)

user1.key = discrete_log(user2.pub, p, user1.pvt)
user2.key = discrete_log(user1.pub, p, user2.pvt)

println(`user1 = ${user1}`)
println(`user2 = ${user2}`)

let dst: i8[1024]

base64(dst, &user1.key, 2)
println(`user1 key = ${dst}`)

base64(dst, &user2.key, 2)
println(`user2 key = ${dst}`)

let src = "Hello World!"
println(`original: ${src}`)

xor(dst, src, &user1.key, 8)
println(`encoded: ${dst}`)

xor(dst, dst, &user2.key, 8)
println(`decoded: ${dst}`)
