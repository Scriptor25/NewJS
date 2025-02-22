extern function rand(): u32

export function random_key(dst: i8[], words: u64): i8[] {
    for (let i: u64; i < words; ++i) {
        (dst as u32[])[i] = rand()
    }
    return dst
}

export function mix_keys(dst: i8[], src1: i8[], src2: i8[], words: u64): i8[] {
    let dst_32: u32[] = dst
    let src1_32: u32[] = src1
    let src2_32: u32[] = src2
    for (let i: u64; i < words; ++i) {
        let t: u32 = 10 ** i
        dst_32[i] = src1_32[i] * t * src2_32[i] * t
    }
    return dst
}

export function base64(dst: i8[], src: i8[], words: u64): i8[] {
    let LUT = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    let src_len = words * 4

    let i: u64
    let j: u64

    let padding = (3 - (src_len % 3)) % 3
    let dst_len = ((src_len + 2) / 3) * 4

    for (; i < src_len;) {
        let octet_a: u64 = i < src_len ? src[i++] : 0
        let octet_b: u64 = i < src_len ? src[i++] : 0
        let octet_c: u64 = i < src_len ? src[i++] : 0

        let triple = (octet_a << 16) | (octet_b << 8) | octet_c

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
