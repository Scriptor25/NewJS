extern rand(): u32

function random_key(dst: i8[], words: u64): i8[] {
    for (let i: u64; i < words; ++i) {
        (dst as u32[])[i] = rand()
    }
    return dst
}

function mix_keys(dst: i8[], src1: i8[], src2: i8[], words: u64): i8[] {
    let dst_32 = dst as u32[]
    let src1_32 = src1 as u32[]
    let src2_32 = src2 as u32[]
    for (let i: u64; i < words; ++i) {
        let t = (10u64 ** i) as u32
        dst_32[i] = src1_32[i] * t * src2_32[i] * t
    }
    return dst
}

function base64(dst: i8[], src: i8[], words: u64): i8[] {
    let LUT = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
    let src_len = words * 4u64

    let i: u64
    let j: u64

    let padding = (3u64 - (src_len % 3u64)) % 3u64
    let dst_len = ((src_len + 2u64) / 3u64) * 4u64

    for (; i < src_len;) {
        let octet_a = (i < src_len) ? (src[i++] as u64) : 0u64
        let octet_b = (i < src_len) ? (src[i++] as u64) : 0u64
        let octet_c = (i < src_len) ? (src[i++] as u64) : 0u64

        let triple = (octet_a << 16u64) | (octet_b << 8u64) | octet_c

         dst[j++] = LUT[(triple >> 18u64) & 0x3Fu64]
         dst[j++] = LUT[(triple >> 12u64) & 0x3Fu64]
         dst[j++] = LUT[(triple >> 6u64) & 0x3Fu64]
         dst[j++] = LUT[triple & 0x3Fu64]
    }

    for (i = 0u64; i < padding; ++i)
        dst[dst_len - 1u64 - i] = '='

    dst[dst_len] = '\x00'
    return dst
}
