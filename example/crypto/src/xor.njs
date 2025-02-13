extern function strlen(str: i8[]): u64

function xor(dst: i8[], src: i8[], key: i8[], key_len: u64): i8[] {
    let src_len = strlen(src)
    for (let i: u64; i < src_len; ++i) {
        dst[i] = src[i] ^ key[i % key_len]
    }
    dst[src_len] = '\x00'
    return dst
}
