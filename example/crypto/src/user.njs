import { random_key } from "./key.njs"

type user_t = {
    pvt: i8[16],
    pub: i8[16],
    key: i8[16],
}

export function create_user(): user_t {
    let user: user_t
    random_key(user.pvt, 4)
    return user
}
