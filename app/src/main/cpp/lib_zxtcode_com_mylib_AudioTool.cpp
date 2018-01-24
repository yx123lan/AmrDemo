#include "lib_zxtcode_com_mylib_AudioTool.h"

void throwExceptionByName(JNIEnv *env, const char *name, const char *msg) {
    jclass cls = env->FindClass(name);
    /* if cls is NULL, an exception has already been thrown */
    if (cls != NULL) {
        env->ThrowNew(cls, msg);
    }
    /* free the local ref */
    env->DeleteLocalRef(cls);
}

JNIEXPORT int JNICALL Java_lib_zxtcode_com_mylib_AudioTool_isAMRFile
        (JNIEnv *env, jclass type, jstring inputFile_) {
    const char *inputFile = env->GetStringUTFChars(inputFile_, 0);

    std::string in(inputFile);
    int stateCode = isAMRFile(in);

    env->ReleaseStringUTFChars(inputFile_, inputFile);
    return stateCode;
}

JNIEXPORT void JNICALL Java_lib_zxtcode_com_mylib_AudioTool_joinAMRFiles
        (JNIEnv *env, jclass type, jobjectArray inputFileArray_, jstring outputFile_) {
    const char *outputFile = env->GetStringUTFChars(outputFile_, 0);
    jint arrayLength = env->GetArrayLength(inputFileArray_);
    // 合并音频文件
    std::vector<std::string> inputFileList;
    for (int i=0; i<arrayLength; i++) {
        jstring inoutFile_ = (jstring) env->GetObjectArrayElement(inputFileArray_, i);
        // 转为C++的String添加到容器中
        std::string inoutFile((char *) env->GetStringUTFChars(inoutFile_, 0));
        inputFileList.push_back(inoutFile);
    }
    int stateCode = joinAMRFiles(inputFileList, outputFile);
    if (stateCode == IO_ERROR) {
        throwExceptionByName(env, "Ljava/lang/IOException;", "can't open inputFile or outputFile");
    }
    // amr文件的格式不一致，不能合并为同一个文件
    if (stateCode == JOIN_FILE_FORMAT_ERROR) {
        throwExceptionByName(env, "Lcom/bloomlife/android/media/JoinFileFailureException;", "files different formats");
    }
    env->ReleaseStringUTFChars(outputFile_, outputFile);
}

JNIEXPORT void JNICALL Java_lib_zxtcode_com_mylib_AudioTool_splitAMRFiles
        (JNIEnv *env, jclass type, jobjectArray outputInfoArray_, jstring inputFile_) {
    jint arrayLength = env->GetArrayLength(outputInfoArray_);
    const char *inputFile = env->GetStringUTFChars(inputFile_, 0);

    std::vector<AudioPartInfo> outputInfoList;
    jmethodID audioTimeMethodId = nullptr;
    jmethodID filePathMethodId = nullptr;
    // 把java层的AudioPartInfo数组转换为c++的AudioPartInfo数组
    for (int i=0; i<arrayLength; i++) {
        jobject obj = env->GetObjectArrayElement(outputInfoArray_, i);
        // 获取方法ID
        if (audioTimeMethodId == nullptr || filePathMethodId == nullptr) {
            jclass cls = env->GetObjectClass(obj);
            audioTimeMethodId = env->GetMethodID(cls, "getAudioTime", "()J");
            filePathMethodId = env->GetMethodID(cls, "getFilePath", "()Ljava/lang/String;");
        }
        // 调用方法获取值
        jlong audioTime_ = env->CallLongMethod(obj, audioTimeMethodId);
        jstring filePath_ = (jstring) env->CallObjectMethod(obj, filePathMethodId);

        // 转为C++的String添加到容器中
        std::string filePath((char *) env->GetStringUTFChars(filePath_, 0));
        AudioPartInfo audioPartInfo((long) audioTime_, filePath);
        outputInfoList.push_back(audioPartInfo);
    }
    int stateCode = splitAMRFiles(outputInfoList, inputFile);
    if (stateCode == IO_ERROR) {
        throwExceptionByName(env, "Ljava/lang/IOException;", "can't open inputFile or outputFile");
    }
    env->ReleaseStringUTFChars(inputFile_, inputFile);
}