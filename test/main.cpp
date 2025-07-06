/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-06 18:41:41
 * @FilePath: \SparseArrayAnalyzer\test\main.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include <iostream>
#include <memory>
#include <vector>
#include "common.h"

// TODO：兼容整形和浮点型

#define FILE_PATH (1)
#define ARRAY_DIMENSION (2)
#define ARRAY_COL (3)
#define ARRAY_ROW (4)

void printUsage()
{
    std::cout << "Usage: sparse_array_analyzer <array.txt> [1/2] [ROW] [COL]\n";
    std::cout << "  1: Analyze as 1D array, eg: <array.txt> 1\n";
    std::cout << "  2: Analyze as 2D array with specified ROWxCOL, eg: <array.txt> 2 9 30\n";
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cerr << "Error: Invalid arguments.\n";
        printUsage();
        return 1;
    }
    printf("Start analyze!\n");

    // 原始数组
    std::vector<uint32_t> data = loadArrayFromTxt(argv[1]);
    std::vector<std::vector<uint32_t>> data2D; // 用于存储二维数组
    uint32_t row = 0, col = 0;

    if (argv[ARRAY_DIMENSION] && std::string(argv[ARRAY_DIMENSION]) == "1")
    {
        // 一维数组
        printf("Input array is 1D array.\n");
        printVector1D(data);
    }
    else if (argv[ARRAY_DIMENSION] && std::string(argv[ARRAY_DIMENSION]) == "2")
    {
        // 二维数组
        printf("Input array is 2D array.\n");
        row = parseInt(argv[ARRAY_ROW]);
        col = parseInt(argv[ARRAY_COL]);
        if(reshapeTo2D(data, row, col, data2D) == SAA_SUCCESS)
        {
            std::cout << "Reshape to 2D array successfully.\n";
            printVector2D(data2D, row, col);
        }
        else
        {
            std::cerr << "Failed to reshape to 2D array. Please check the input format.\n";
            return 1;
        }
    }
    else
    {
        std::cerr << "Invalid dimension argument. Use 1 for 1D or 2 for 2D.\n";
        return 1;
    }

    /* -------------------------------- main func ------------------------------- */
    // 1. 加载数组数据
    // std::vector<uint32_t> data = loadArrayFromTxt(argv[1]);
    if (data.empty())
    {
        std::cerr << "Failed to load data or file is empty.\n";
        return 1;
    }

    // 2. 构造参数结构体
    // SparseArrayCompressor::Params params;
    // params.dataPtr = reinterpret_cast<const uint8_t *>(data.data());
    // params.dataSize = static_cast<uint32_t>(data.size() * sizeof(uint32_t));
    // params.elementSize = sizeof(uint32_t);
    // params.typeName = "uint32_t";

    // 3. 获取所有已注册算法
    const auto &allModes = CompressorRegistry::Instance().ListAlgorithms();

    std::cout << "== Compression Comparison Report ==\n";
    std::cout << "Input size: " << data.size() << " elements\n\n";

    // 4. 遍历每种压缩算法进行测试
    for (const auto &mode : allModes)
    {
        auto compressor = CompressorRegistry::Instance().Create(mode);
        if (!compressor)
        {
            std::cerr << "[WARN] Compressor \"" << mode << "\" not found.\n";
            continue;
        }

        bool ok = compressor->Compress(data);
        if (!ok)
        {
            std::cout << mode << ": Compression failed.\n";
            continue;
        }

        // TODO: "\n" 和 endl
        CalResult result = compressor->GetResult();
        std::cout << "  Compressed Mode  : " << mode << "\n";
        std::cout << "  Origin count     : " << result.originElementCount << "\n";
        std::cout << "  Compressed count : " << result.compressedElementCount << "\n";
        std::cout << "  Origin size      : " << result.originSizeBytes << " bytes\n";
        std::cout << "  Compressed Size  : " << result.compressedSizeBytes << " bytes\n";
        std::cout << "  Compress Time    : " << result.compressTimeMs << " us\n";
        std::cout << "  Decompress Time  : " << result.decompressTimeMs << " us\n";
        std::cout << "  Ratio            : " << result.compressionRatio << "\n\n";
    }
    /* ----------------------------------- end ---------------------------------- */
    return 0;
}
