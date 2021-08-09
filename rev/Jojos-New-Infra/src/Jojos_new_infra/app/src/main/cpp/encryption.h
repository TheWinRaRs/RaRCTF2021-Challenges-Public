#include "imports.h"

#define CBC 1
#define CTR 0
#define ECB 0

/*
 Base64 decode - manipulated charset so not easily recoverable apart from running it yourself
 */
static std::string base64_decode(const std::string &in) {
    std::string out;
    std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-@abcdefghijklmnopqrstuvwxyz";

    std::vector<int> T(256,-1);
    for (int i=0; i<64; i++) T[charset[i]] = i;

    int val=0, valb=-8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val>>valb)&0xFF));
            valb -= 8;
        }
    }
    return out;
}

/*
 XOR encryption and decryption
 */
std::string encryptDecryptUsername(std::string key, std::string toEncrypt) {
    std::string output = toEncrypt;

    for (int i = 0; i < toEncrypt.size(); i++)
        output[i] = toEncrypt[i] ^ key[i];

    return output;
}

/*
 Decrypting key used for AES-encrypted password
 */
std::vector<int> decryptKey() {
    unsigned int encryptedKey[] = { 4282382796, 4294967259, 4287148280, 4294967081, 3718282461, 4294967240, 2318534144, 4294967290, 632638938, 4294967167,
                                    3188835952, 4294967110, 1896266778, 4294967243, 1696976455, 4294967124, 2587123341, 4294967050, 1717499897, 4294967207,
                                    2785188224, 4294967220, 109486945, 4294967239, 4186300312, 4294967176, 3647649858, 4294967067, 648068912, 4294967230,
                                    473488344, 4294967142 };

    std::vector<unsigned int> encryptedKeyVector(encryptedKey, encryptedKey + 32);
    std::vector<signed int> key(encryptedKeyVector.size() / 2, '0');

    for (std::vector<int>::size_type i = 0; i != encryptedKeyVector.size(); i++) {
        key[i / 2] = ~encryptedKeyVector[i] & 0xffffffff;
    }
    return key;
}

/*
 Decrypt AES-encrypted password
 */
std::string aes_cbc_decrypt() {
    std::vector<int> decryptedKey = decryptKey();
    std::vector<unsigned char> keyChar;

    for (int i : decryptedKey) {
        keyChar.push_back(i);
    }

    uint8_t key[16];
    for (int i = 0; i < keyChar.size(); i++) {
        key[i] = keyChar[i];
    }

    std::string origIV = base64_decode("Gy7FbIR1otHNNjnXZ9z8j6::");
    std::vector<unsigned char> ivVector(origIV.begin(), origIV.end());
    uint8_t iv[16];
    for (int i = 0; i < ivVector.size(); i++) {
        iv[i] = ivVector[i];
    }

    //uint8_t key[] = { 0x24, 0xd6, 0x37, 0x5, 0x80, 0xb9, 0x34, 0xab, 0xf5, 0x58, 0x4b, 0x38, 0x77, 0xe4, 0x41, 0x99 };
    uint8_t in[]  = { 0xb5, 0x76, 0x19, 0xea, 0xee, 0xdc, 0x2d, 0xaa, 0xd2, 0x7b, 0xd0, 0xc3, 0x4b, 0x9a, 0xb3, 0x3f, 0x93,
                      0xfc, 0x66, 0x3d, 0xf2, 0xc0, 0x8c, 0xf, 0x58, 0x37, 0xd, 0x19, 0x85, 0x3a, 0x70, 0x5a };
    //uint8_t iv[]  = { 0x1b, 0xe8, 0x45, 0x9c, 0x84, 0x5b, 0xd3, 0x91, 0xcd, 0x36, 0xfc, 0xd7, 0x66, 0x3f, 0xe2, 0xbe };

    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, in, 24);

    std::string pwd( (const char *) in);

    return pwd.substr(0, 23);
}
