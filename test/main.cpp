/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-18 19:00:11
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
    std::cout << COLOR_STR("Usage:", COLOR_BLUE) << "sparse_array_analyzer <array.txt> [1/2] [ROW] [COL]\n";
    std::cout << "  1: Analyze as 1D array, eg: <array.txt> 1\n";
    std::cout << "  2: Analyze as 2D array with specified ROWxCOL, eg: <array.txt> 2 9 30\n";
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        if (argc == 2 && std::string(argv[FILE_PATH]) == "--help")
        {
            printUsage();
            return 0;
        }
        else if (argc == 3 && std::string(argv[1]) == "--version")
        {
            std::cout << "Sparse Array Analyzer v1.0.0\n";
            return 0;
        }
        else if (argv[ARRAY_DIMENSION] && std::string(argv[ARRAY_DIMENSION]) == "1")
        {
            ;
        }
        else
        {
            std::cerr << LOG_ERROR << "Invalid arguments.\n";
            printUsage();
            return 0;
        }
    }
    printf(COLOR_GREEN "======= [Start analyze!] =======" COLOR_RESET "\n");

    // 原始数组
    std::vector<uint32_t> data = LoadArrayFromTxt(argv[FILE_PATH]);

    ArrayData1D inputData1D;
    ArrayData2D inputData2D;
    ArrayDimension inputDimension = ARRAY_1D;

    if (argv[ARRAY_DIMENSION] && std::string(argv[ARRAY_DIMENSION]) == "1")
    {
        // 一维数组
        printf("Input array is 1D array.\n");
        inputData1D.arrayData = data;
        PrintVector1D(data);
    }
    else if (argv[ARRAY_DIMENSION] && std::string(argv[ARRAY_DIMENSION]) == "2")
    {
        // 二维数组
        printf("Input array is 2D array.\n");
        inputData2D.rowCount = ParseInt(argv[ARRAY_ROW]);
        inputData2D.colCount = ParseInt(argv[ARRAY_COL]);
        if(ReshapeTo2D(data, inputData2D.rowCount, inputData2D.colCount, inputData2D.arrayData) == SAA_SUCCESS)
        {
            std::cout << "Reshape to 2D array successfully.\n";
            PrintVector2D(inputData2D.arrayData, inputData2D.rowCount, inputData2D.colCount);
            inputDimension = ARRAY_2D;
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

    // 2. 构造参数结构体
    // SparseArrayCompressor::Params params;
    // params.dataPtr = reinterpret_cast<const uint8_t *>(data.data());
    // params.dataSize = static_cast<uint32_t>(data.size() * sizeof(uint32_t));
    // params.elementSize = sizeof(uint32_t);
    // params.typeName = "uint32_t";

    // 3. 获取所有已注册算法
    const auto &allModes = CompressorRegistry::Instance().ListAlgorithms();

    std::cout << COLOR_STR("==== Compression Comparison Report ====", COLOR_PURPLE) << "\n";
    std::cout << "Input size: " << COLOR_BLUE << data.size() << COLOR_RESET << "elements\n\n";

    ArrayInput input = (inputDimension == ARRAY_1D) ? ArrayInput{inputData1D }: ArrayInput{inputData2D};

    // 4. 遍历每种压缩算法进行测试
    for (const auto &mode : allModes)
    {
        auto compressor = CompressorRegistry::Instance().Create(mode);
        if (!compressor)
        {
            std::cerr << LOG_WARN << "Compressor \"" << mode << "\" not found.\n";
            continue;
        }

        int8_t ret = compressor->Compress(input);
        if (ret != SAA_SUCCESS)
        {
            std::cerr << LOG_ERROR << "Compression failed for " << mode << ". Error code: " << static_cast<int>(ret) << "\n";
            continue;
        }

        ret = compressor->Decompress(input);
        if (ret != SAA_SUCCESS)
        {
            std::cerr << LOG_ERROR << "Decompression failed for " << mode << ". Error code: " << static_cast<int>(ret) << "\n";
            continue;
        }

        // TODO: "\n" 和 endl
        CalResult result;
        if(compressor->GetResult(result) != SAA_SUCCESS)
        {
            std::cerr << LOG_ERROR <<"Failed to get compression result for " << mode << ".\n";
            continue;
        }

        std::cout << "[ Compressed Mode  ] " << COLOR_STR(mode, COLOR_BLUE) << "\n";
        std::cout << "  Origin count     : " << result.originElementCount << "\n";
        std::cout << "  Compressed count : " << result.compressedElementCount << "\n";
        std::cout << "  Origin size      : " << result.originSizeBytes << " bytes\n";
        std::cout << "  Compressed Size  : " << result.compressedSizeBytes << " bytes\n";
        std::cout << "  Compress Time    : " << result.compressTimeMs << " ms\n";
        std::cout << "  Decompress Time  : " << result.decompressTimeMs << " ms\n";
        std::cout << "  Ratio            : " << result.compressionRatio << " %\n\n";
    }
    /* ----------------------------------- end ---------------------------------- */
    return 0;
}
