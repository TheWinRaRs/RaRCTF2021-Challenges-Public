import os

fn check(inp string) bool {
    mut a := []byte{}
    mut key := [0x13, 0x37]
    for c in inp {
        a << byte((c ^ key[0]) + key[1])
        key = [key[1], key[0]]
    }
    if a.map(fn (w byte) int {return int(w)}) == [0x98, 0x69, 0x98, 0x67, 0x9e, 0x64, 0x9f, 0x77, 0xad, 0x65, 0x76, 0x76, 0xb2, 0x69, 0x9e, 0x73, 0xa9, 0x57, 0xb4, 0x23, 0x9e, 0x77, 0xb3, 0x92, 0xa9, 0x58, 0xae, 0x2d, 0x59, 0x65, 0xa8, 0x15, 0x59, 0x21, 0xad, 0x66, 0xa5] {
        return true
    }
    return false
}

fn main() {
    inp := os.input("Attempt flag: ")
    if check(inp) {
        println('yes')
    }
}
