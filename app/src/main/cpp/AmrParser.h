//
// Created by cjf on 2017/5/2.
//
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>

#ifndef APP_AMRPARSER_H
#define APP_AMRPARSER_H

class AmrParser {

public:
    // 文件太小了
    static const int FILE_TOO_SMALL_TO_PARES = -1;
    // AMR文件读取完毕
    static const int READ_FILE_FINISH = 0;
    // 未知格式
    static const int UNSUPPORTED_FORMAT = -2;

    static const int AMR_NB = 1;
    static const int AMR_WB = 2;

    // 采样率
    static const int AMR_NB_SAMPLE_RATE = 8000;
    static const int AMR_WB_SAMPLE_RATE = 16000;

    // AMR音频帧大小
    static const int AMR_NB_BLOCK_SIZES[];
    static const int AMR_WB_BLOCK_SIZES[];

    // AMR的文件头
    static const char AMR_NB_HEADER[];
    static const char AMR_WB_HEADER[];

    AmrParser();

    explicit AmrParser(std::string &inputFilePath);

    // 禁止拷贝构造函数
    AmrParser(const AmrParser &parser) = delete;
    // 禁止赋值构造函数
    AmrParser &operator=(const AmrParser &parser) = delete;

    int readFile(std::string &inputFilePath);

    int getSampleRate();

    int getNumFrames();

    int getSamplesPerFrame();

    int getChannels();

    long getFileSizeBytes();

    int getFileType();

    std::vector<int> getFrameLens();

    std::vector<int> getFrameOffsets();

    ~AmrParser();

private:
    int numFrames;

    std::vector<int> frameOffsets;
    std::vector<int> frameLens;

    int sampleRate;

    int offset;
    long fileLength;
    int fileType;

    void parseAMR(std::fstream &inputFile, long maxLength);
    long parseAMRFrame(std::fstream &inputFile, long maxLength);
    void addFrame(int frameOffset, int frameLen);
};

#endif //APP_AMRPARSER_H