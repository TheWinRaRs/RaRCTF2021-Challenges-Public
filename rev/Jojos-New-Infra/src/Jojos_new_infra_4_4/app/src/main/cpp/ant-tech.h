#include "imports.h"

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_jojos_1new_1infra_14_14_checkStuff_cS(
        JNIEnv *env,
        jobject thiz,
        jstring javaSignature,
        jboolean debugModeSet) {
    std::string SIGNATURE;
    if (debugModeSet) {
        SIGNATURE = encryptDecryptUsername("LrGdetLPwTN1FEkbMuPQ0510H3Ast", ">03QQ_91!?*b1&8(*;f$qGH|:t2N~");
    } else {
        SIGNATURE = encryptDecryptUsername("2F9gVL021dCMCW1QCw8rdsCg9mvm", "^>vW;?gxX(w+%!B5h#Z3V>(2R!7P");
    }
    std::string signature_java = std::string(env->GetStringUTFChars(javaSignature, nullptr));
    //SIGNATURE.erase(std::remove(SIGNATURE.begin(), SIGNATURE.end(), '\n'),SIGNATURE.end());
    //__android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "Signature native = %s", SIGNATURE.c_str());
    return SIGNATURE == signature_java;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_jojos_1new_1infra_14_14_checkStuff_ctp(JNIEnv *env, jobject thiz) {
    std::ifstream status_file;
    std::string line;
    std::string delim = ":";
    pid_t PID = getpid();
    std::string filepath = "/proc/" + std::to_string(PID) + "/status";
    status_file.open(filepath);
    if (status_file.is_open()) {
        while ( getline(status_file, line) ) {
            std::string key = line.substr(0, line.find(delim));
            if (key == "TracerPid") {
                std::string value = line.substr(line.find(delim) + 1, line.length());
                value.erase(value.begin());
                return (value == "0");
            }
        }
        status_file.close();
    } else {
        __android_log_print(ANDROID_LOG_DEBUG, "DEBUGGING", "%s not opened", filepath.c_str());
        return false;
    }
    return false;
}