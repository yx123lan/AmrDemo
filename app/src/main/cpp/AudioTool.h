//
// Created by tarent on 2017/5/1.
//
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

//#include "gain_control.h"
//#include "noise_suppression.h"
#include "AmrParser.h"
#include "AudioPartInfo.h"

#ifndef APP_AUDIO_TOOL_H
#define APP_AUDIO_TOOL_H
#ifdef __cplusplus
extern "C" {
#endif
const int SUCCESS = 0;
const int INIT_NOISE_PROCESS_ERROR = -5;
const int JOIN_FILE_FORMAT_ERROR = -4;
const int NO_AMR = -3;
const int INPUT_EMPTY = -2;
const int IO_ERROR = -1;

int webRtcNoiseProcess(std::string &inputFile, std::string &outputFile, uint32_t sampleRate);
int isAMRFile(std::string &outputFile);
int joinAMRFiles(std::vector<std::string> &inputFileList, const char *outputFileName);
int createAMRPart(std::string &outputFile, std::string &inputFile, AmrParser &amrParser, int startFrame, int numFrames);
int splitAMRFiles(std::vector<AudioPartInfo> &splitFileInfoList, std::string inputFile);
#ifdef __cplusplus
}
#endif
#endif //APP_AUDIO_TOOL_H
