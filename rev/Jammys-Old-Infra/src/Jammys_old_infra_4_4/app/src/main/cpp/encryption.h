#include <jni.h>
#include <vector>
#include <string>

#define CBC 1
#define CTR 0
#define ECB 0

#include "aes.hpp"

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
std::string encryptDecryptUsername(std::string toEncrypt) {
    std::string key = "bc79T08j";
    std::string output = toEncrypt;

    for (int i = 0; i < toEncrypt.size(); i++)
        output[i] = toEncrypt[i] ^ key[i];

    return output;
}

/*
 Decrypting key used for AES-encrypted password
 */
std::vector<int> decryptKey() {
    unsigned int encryptedKey[] = { 4282382796, 4294967246, 4287148280, 4294967128, 3718282461, 4294967193, 2318534144,
                                    4294967241, 632638938, 4294967208, 3188835952, 4294967090, 1896266778, 4294967280,
                                    1696976455, 4294967282, 2587123341, 4294967236, 1717499897, 4294967190, 2785188224,
                                    4294967077, 109486945, 4294967233, 4186300312, 4294967188, 3647649858, 4294967166,
                                    648068912, 4294967201, 473488344, 4294967198 };

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

    std::string origIV = base64_decode("fVklgASTzDzdjhh4SOPJR6::");
    std::vector<unsigned char> ivVector(origIV.begin(), origIV.end());
    uint8_t iv[16];
    for (int i = 0; i < ivVector.size(); i++) {
        iv[i] = ivVector[i];
    }

    //uint8_t key[] = { 0x31, 0xa7, 0x66, 0x36, 0x57, 0xcd, 0x0f, 0x0d, 0x3b, 0x69, 0xda, 0x3e, 0x6b, 0x81, 0x5e, 0x61 };
    uint8_t in[]  = { 0xb3, 0x7f, 0x03, 0xb5, 0xd8, 0x90, 0x86, 0xf4, 0xc2, 0x97, 0x18, 0x97, 0x87, 0x77, 0x6d, 0x1d,
                      0x6a, 0x9c, 0x4d, 0xe5, 0x73, 0x58, 0xc6, 0x7d, 0x9d, 0x92, 0xf0, 0xd9, 0x81, 0x66, 0x4f, 0xa5 };
    //uint8_t iv[]  = { 0xad, 0x5c, 0x31, 0xb0, 0x04, 0x93, 0xfc, 0x3f, 0xe9, 0xbe, 0xdb, 0x5e, 0x48, 0xe3, 0xc9, 0x46 };

    struct AES_ctx ctx;

    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, in, 24);

    std::string pwd( (const char *) in);

    return pwd.substr(0, 23);
}