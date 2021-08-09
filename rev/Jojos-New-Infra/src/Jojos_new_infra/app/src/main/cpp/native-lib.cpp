#include "imports.h"
#include "encryption.h"
#include "ant-tech.h"

// username check
bool checkUsername(JNIEnv * env, jobject jobject1, jstring jstr_username) {
    std::string name = "!N+-G;P4A$_^DSFW";
    std::string key = "O!aL*V)w.@:hsawd";
    auto adminUsername = encryptDecryptUsername(key, name);
    const char* nativeUname = env->GetStringUTFChars(jstr_username, 0);
    bool result = adminUsername == nativeUname;
    env->ReleaseStringUTFChars(jstr_username, nativeUname);
    return result;
}

// password checks
bool checkPasswords(JNIEnv *env, jstring jstr_pass) {
    std::string nativePass = std::string(env->GetStringUTFChars(jstr_pass, nullptr));
    //__android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Pwd = %s", aes_cbc_decrypt().c_str());
    return nativePass == aes_cbc_decrypt();
}

// main function
extern "C" JNIEXPORT jboolean JNICALL
Java_com_example_jojos_1new_1infra_14_14_checkStuff_cC(
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
