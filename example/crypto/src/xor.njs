extern function strlen(x: string): u64

export function xor(dst: i8[], src: string, key: string, key_len: u64): string {
    let src_len = strlen(src)
    for (let i: u64; i < src_len; ++i) {
        dst[i] = src[i] ^ key[i % key_len]
    }
    dst[src_len] = '\x00'
    return dst
}
