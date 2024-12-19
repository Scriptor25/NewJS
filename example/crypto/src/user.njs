import { random } from "./random.njs"

type user_t = {
    pvt: u64,
    pub: u64,
    res: u64,
}

function create_user(): user_t {
    let user: user_t
    user.pvt = random()
    return user
}
