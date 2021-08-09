#include <jni.h>
#include "encryption.h"

// username check
bool checkUsername(JNIEnv * env, jobject jobject1, jstring jstr_username) {
    std::string name = "SQ]X9]A>";
    auto adminUsername = encryptDecryptUsername(name);
    const char* nativeUname = env->GetStringUTFChars(jstr_username, 0);
    bool result = adminUsername == nativeUname;
    env->ReleaseStringUTFChars(jstr_username, nativeUname);
    return result;
}

// password checks
bool checkPasswords(JNIEnv *env, jstring jstr_pass) {
    std::string nativePass = std::string(env->GetStringUTFChars(jstr_pass, nullptr));
    return nativePass == aes_cbc_decrypt();
}

// main function
extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_jammys_1old_1infra_14_14_MainActivity_checkCreds(
        JNIEnv* env,
        jobject obj, /* this */
        jstring username,
        jstring password,
        jint lookingFor) {
    if (username == nullptr || password == nullptr) { return false; }

    bool result;

    switch (lookingFor) {
        case 0: {
            result = checkUsername(env, obj, username);
            break;
        }
        case 1: {
            result = checkPasswords(env, password);
            break;
        }
        default: {
            result = false;
            break;
        }
    }
    return result;
}