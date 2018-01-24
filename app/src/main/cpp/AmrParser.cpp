#include "AmrParser.h"

const int AmrParser::AMR_NB_BLOCK_SIZES[] = {
        12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0
};
const int AmrParser::AMR_WB_BLOCK_SIZES[] = {
        17, 22, 32, 36, 40, 46, 50, 58, 60, 0, 0, 0, 0, 0, 0, 0
};

const char AmrParser::AMR_NB_HEADER[] = "#!AMR\n";

const char AmrParser::AMR_WB_HEADER[] = "#!AMR-WB\n";

AmrParser::AmrParser() {

}

AmrParser::AmrParser(std::string &inputFilePath) {
    readFile(inputFilePath);
}

int AmrParser::readFile(std::string &inputFilePath) {
    numFrames = 0;
    std::fstream inputFile;
    inputFile.open(inputFilePath.c_str(), std::ios::binary | std::ios::in);
    // 获取文件长度
    auto start = inputFile.tellg();
    inputFile.seekg(0, std::ios::end);
    fileLength = inputFile.tellg() - start;

    inputFile.seekg(0, std::ios::beg);
    offset = 0;
    if (fileLength < 128) {
        return FILE_TOO_SMALL_TO_PARES;
    }
    // 根据不同的AMR类型去解析
    char header[9];
    inputFile.read(header, 6);
    offset += 6;
    if (header[0] == '#' &&
        header[1] == '!' &&
        header[2] == 'A' &&
        header[3] == 'M' &&
        header[4] == 'R' &&
        header[5] == '\n') {
        sampleRate = AMR_NB_SAMPLE_RATE;
        fileType = AMR_NB;
        parseAMR(inputFile, fileLength - 6);
        inputFile.close();
        return READ_FILE_FINISH;
    }
    inputFile.read(header + 6, 3);
    offset += 3;
    if (header[0] == '#' &&
        header[1] == '!' &&
        header[2] == 'A' &&
        header[3] == 'M' &&
        header[4] == 'R' &&
        header[5] == '-' &&
        header[6] == 'W' &&
        header[7] == 'B' &&
        header[8] == '\n') {
        sampleRate = AMR_WB_SAMPLE_RATE;
        fileType = AMR_WB;
        parseAMR(inputFile, fileLength - 9);
        inputFile.close();
        return READ_FILE_FINISH;
    }
    inputFile.close();
    return UNSUPPORTED_FORMAT;
}

void AmrParser::parseAMR(std::fstream &inputFile, long maxLength) {
    while (maxLength > 0) {
        long bytesConsumed = parseAMRFrame(inputFile, maxLength);
        maxLength -= bytesConsumed;
    }
}

/**
 *
 * @param inputFile AMR文件
 * @param maxLength 文件剩余的最大长度
 * @return 当前音频帧的大小
 */
long AmrParser::parseAMRFrame(std::fstream &inputFile, long maxLength) {
    int frameOffset = offset;
    char frameTypeHeader[1];
    inputFile.read(frameTypeHeader, 1);
    offset += 1;
    // 获取当前音频帧的类型
    int frameType = ((0xff & frameTypeHeader[0]) >> 3) % 0x0F;
    // 查询当前音频帧的大小
    int blockSize = fileType == AMR_WB ? AMR_WB_BLOCK_SIZES[frameType] : AMR_NB_BLOCK_SIZES[frameType];
    if (blockSize + 1 > maxLength) {
        // 如果要读的这个帧超过剩余文件长度，那么相当于读取完剩余的文件
        return maxLength;
    }
    if (blockSize == 0) {
        return 1;
    }
    char block[blockSize];
    inputFile.read(block, blockSize);
    offset += blockSize;

    addFrame(frameOffset, blockSize + 1);

    return blockSize + 1;
}

/**
 *
 * @param frameOffset 当前音频帧的从文件头开始的偏移量
 * @param frameLen 音频帧大小
 */
void AmrParser::addFrame(int frameOffset, int frameLen) {
    frameOffsets.push_back(frameOffset);
    frameLens.push_back(frameLen);
    numFrames++;
}

int AmrParser::getSampleRate() {
    return sampleRate;
}

int AmrParser::getNumFrames() {
    return numFrames;
}

int AmrParser::getSamplesPerFrame() {
    return 50;
}

int AmrParser::getChannels() {
    return 1;
}

long AmrParser::getFileSizeBytes() {
    return fileLength;
}

int AmrParser::getFileType() {
    return fileType;
}

std::vector<int> AmrParser::getFrameLens() {
    return frameLens;
}

std::vector<int> AmrParser::getFrameOffsets() {
    return frameOffsets;
}

AmrParser::~AmrParser() {
}