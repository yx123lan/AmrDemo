//
// Created by cjf on 2017/5/2.
//

#include "AudioPartInfo.h"

AudioPartInfo::AudioPartInfo(long audioTime, std::string &filePath) {
    this->audioTime = audioTime;
    this->filePath = filePath;
}

long AudioPartInfo::getAudioTime() {
    return audioTime;
}

std::string& AudioPartInfo::getFilePath() {
    return filePath;
}

AudioPartInfo::~AudioPartInfo() {

}