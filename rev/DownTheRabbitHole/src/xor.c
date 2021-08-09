#define NAND(X, Y) ~(X&Y)
#define NANDRANGE(dst, s1, s2, len) for(int i = 0; i < len; i ++) { dst[i] = NAND(s1[i], s2[i]); }
#define XOR(X, Y) NAND(NAND(X, NAND(X, Y)), NAND(Y, NAND(X, Y)));
#define XORRANGE(dst, s1, s2, len) for(int i = 0; i < len; i ++) { dst[i] = XOR(s1[i], s2[i]); }

void xor(char *dest, char* str1, char* str2, int length, int mode) {
    if (mode == 0) {
        for (int i=0; i<length; i++) {
            dest[i] = str1[i] ^ str2[i] ^ 0x90;
            // is this obfuscation :butterfly:
        }
    } else {
        XORRANGE(dest, str1, str2, length);
    }
}

