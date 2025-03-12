extern function rand(): i32

export function random(): u64 {
    return (rand() as u64) << 32 | (rand() as u64)
}

export function discrete_log(g: u64, p: u64, x: u64): u64 {
    let result: u64 = 1
    g %= p
    while (x > 0) {
        if (x & 1)
            result = (result * g) % p
        g = (g * g) % p
        x >>= 1
    }
    return result
}

export function base64(dst: i8[], src: string, words: u64): string {
    const LUT = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    const src_len = words * 4

    let i: u64
    let j: u64

    const padding = (3 - (src_len % 3)) % 3
    const dst_len = ((src_len + 2) / 3) * 4

    for (; i < src_len;) {
        const octet_a: u64 = i < src_len ? src[i++] : 0
        const octet_b: u64 = i < src_len ? src[i++] : 0
        const octet_c: u64 = i < src_len ? src[i++] : 0

        const triple = (octet_a << 16) | (octet_b << 8) | octet_c

         dst[j++] = LUT[(triple >> 18) & 0x3F]
         dst[j++] = LUT[(triple >> 12) & 0x3F]
         dst[j++] = LUT[(triple >>  6) & 0x3F]
         dst[j++] = LUT[triple         & 0x3F]
    }

    for (i = 0; i < padding; ++i)
        dst[dst_len - 1 - i] = '='

    dst[dst_len] = '\x00'
    return dst
}
