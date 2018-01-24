//
// Created by cjf on 2017/5/2.
//

#ifndef BASEFRAME_ANDROID_SPLITFILEINFO_H
#define BASEFRAME_ANDROID_SPLITFILEINFO_H


#include <string>

class AudioPartInfo {
public:
    AudioPartInfo(long audioTime, std::string &filePath);
    long getAudioTime();
    std::string& getFilePath();
    ~AudioPartInfo();
private:
    long audioTime;
    std::string filePath;
};


#endif //BASEFRAME_ANDROID_SPLITFILEINFO_H
