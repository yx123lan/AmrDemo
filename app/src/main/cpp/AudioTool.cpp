#include "AudioTool.h"

/**
 *  判断是AMR-NB还是AMR-WB
 * @param inputFile
 * @return
 */
int isAMRFile(std::string &inputFile) {
    std::fstream file;
    file.open(inputFile.c_str(), std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        return IO_ERROR;
    }
    size_t maxMagicLength = strlen(AmrParser::AMR_NB_HEADER);
    char magic[maxMagicLength];
    int isAmr = NO_AMR;
    file.read(magic, maxMagicLength);
    if (!strncmp(magic, AmrParser::AMR_NB_HEADER, maxMagicLength)) {
        isAmr = AmrParser::AMR_NB;
    } else if (!strncmp(magic, AmrParser::AMR_WB_HEADER, maxMagicLength)) {
        isAmr = AmrParser::AMR_WB;
    }
    file.close();
    return isAmr;
}

/**
 *
 * @param inputFileList 需要合并的文件列表
 * @param outputFileName 合并后输出的文件
 * @return 0 SUCCESS -1 IO_ERROR -2 INPUT_EMPTY
 */
int joinAMRFiles(std::vector<std::string> &inputFileList, const char *outputFileName) {
    if (inputFileList.empty()) {
        return INPUT_EMPTY;
    }
    std::fstream outputFile;
    outputFile.open(outputFileName, std::ios::binary | std::ios::out);
    if (!outputFile.is_open()) {
        return IO_ERROR;
    }
    // 取第一个文件作为参考
    int fileType = isAMRFile(inputFileList[0]);
    // 写入amr文件头
    if (fileType == AmrParser::AMR_NB) {
        outputFile.write(AmrParser::AMR_NB_HEADER, strlen(AmrParser::AMR_NB_HEADER));
    } else {
        outputFile.write(AmrParser::AMR_WB_HEADER, strlen(AmrParser::AMR_WB_HEADER));
    }
    int stateCode = SUCCESS;
    // 缓冲数组
    size_t bufferLength = 8196;
    char buffer[bufferLength];
    for (auto inputFileName = inputFileList.begin(); inputFileName != inputFileList.end(); inputFileName++) {
        int currentType = isAMRFile(*inputFileName);
        if (currentType != fileType) {
            stateCode = JOIN_FILE_FORMAT_ERROR;
            break;
        }
        std::fstream inputFile;
        inputFile.open(*inputFileName, std::ios::binary | std::ios::in);
        // 要合并的文件是否能正常打开
        if (!inputFile.is_open()) {
            continue;
        }
        if (fileType == AmrParser::AMR_NB) {
            inputFile.seekg(strlen(AmrParser::AMR_NB_HEADER), std::ios::cur);
        } else {
            inputFile.seekg(strlen(AmrParser::AMR_WB_HEADER), std::ios::cur);
        }
        while(!inputFile.eof()) {
            inputFile.read(buffer, bufferLength);
            outputFile.write(buffer, inputFile.gcount());
        }
        inputFile.close();
    }
    outputFile.close();
    return stateCode;
}

/**
 *
 * @param splitFileInfoList 需要分割的文件信息列表
 * @param inputFile 被分割的文件
 * @return
 */
int splitAMRFiles(std::vector<AudioPartInfo> &splitFileInfoList, std::string inputFile) {
    AmrParser parser(inputFile);
    int startFrame = 0;
    int stateCode = 0;
    for (auto fileInfo = splitFileInfoList.begin(); fileInfo != splitFileInfoList.end(); fileInfo++) {
        long position = fileInfo->getAudioTime();
        int endFrame = (int)(position / 1000.0 * parser.getSamplesPerFrame() + 0.5);
        if (endFrame > parser.getNumFrames()) {
            endFrame = parser.getNumFrames();
        }
        int numFrame = endFrame - startFrame;
        if (numFrame == 0) {
            break;
        }
        stateCode = createAMRPart(fileInfo->getFilePath(), inputFile, parser, startFrame, numFrame);
        if (stateCode < 0) {
            break;
        }
        startFrame = endFrame;
    }
    return stateCode;
}
/**
 *
 * @param outputFile 原文件路径
 * @param inputFile 要生成的片段路径
 * @param amrParser amr文件信息
 * @param startFrame 要生成的AMR片段在原文件的起始位置
 * @param numFrames 要生成的AMR片段一共有多少帧
 * @return
 */
int createAMRPart(std::string &outputFile, std::string &inputFile, AmrParser &amrParser, int startFrame, int numFrames) {
    std::fstream outFile;
    outFile.open(outputFile.c_str(), std::ios::binary | std::ios::out);
    if (!outFile.is_open()) {
        return IO_ERROR;
    }
    std::fstream inFile;
    inFile.open(inputFile.c_str(), std::ios::binary | std::ios::in);
    if (!inFile.is_open()) {
        return IO_ERROR;
    }
    if (amrParser.getFileType() == AmrParser::AMR_WB) {
        outFile.write(AmrParser::AMR_WB_HEADER, strlen(AmrParser::AMR_WB_HEADER));
    } else {
        outFile.write(AmrParser::AMR_NB_HEADER, strlen(AmrParser::AMR_NB_HEADER));
    }
    int maxFrameLen = 0;
    std::vector<int> frameLens = amrParser.getFrameLens();
    std::vector<int> frameOffsets = amrParser.getFrameOffsets();
    for (int i = 0; i < numFrames; i++) {
        if (frameLens[startFrame + i] > maxFrameLen) {
            maxFrameLen = frameLens[startFrame + i];
        }
    }
    char buffer[maxFrameLen];
    int pos = 0;
    for (int i = 0; i < numFrames; i++) {
        int skip = frameOffsets[startFrame + i] - pos;
        int len = frameLens[startFrame + i];
        if (skip < 0) {
            continue;
        }
        if (skip > 0) {
            inFile.seekg(skip, std::ios::cur);
            pos += skip;
        }
        inFile.read(buffer, len);
        outFile.write(buffer, len);
        pos += len;
    }
    inFile.close();
    outFile.close();
    return SUCCESS;
}