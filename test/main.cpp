/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-08-16 10:13:38
 * @FilePath: \SparseArrayAnalyzer\test\main.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include <iostream>
#include <memory>
#include <vector>
#include <iomanip>
#include <sstream>
#include <string>
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

// 适用于整数
std::string FormatWithUnit(uint32_t value, const std::string &unit, size_t totalWidth)
{
    std::ostringstream oss;
    oss << value << " " << unit;
    std::string result = oss.str();
    if (result.size() < totalWidth)
        result += std::string(totalWidth - result.size(), ' ');
    return result;
}

// 适用于浮点数（保留 n 位小数）
std::string FormatWithUnit(double value, const std::string &unit, size_t totalWidth, int precision = 3)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value << " " << unit;
    std::string result = oss.str();
    if (result.size() < totalWidth)
        result += std::string(totalWidth - result.size(), ' ');
    return result;
}

void PrintResultTable(const std::vector<CalResult> &results)
{
    // 表头
    std::cout << std::left
              << std::setw(24) << "Algorithm"
              << std::setw(10) << "Count"
              << std::setw(18) << "Compressed Cnt"
              << std::setw(15) << "Origin Size"
              << std::setw(18) << "Compressed Size"
              << std::setw(18) << "Compress Time"
              << std::setw(18) << "Decompress Time"
              << std::setw(10) << "Ratio"
              << "\n";

    std::cout << std::string(134, '-') << "\n";

    // 每一行输出一个 CalResult
    for (const auto &result : results)
    {
        std::cout << std::left
                  << std::setw(24) << result.modeName
                  << std::setw(10) << result.originElementCount
                  << std::setw(18) << result.compressedElementCount
                  << FormatWithUnit(result.originSizeBytes, "Byte", 15)
                  << FormatWithUnit(result.compressedSizeBytes, "Byte", 18)
                  << FormatWithUnit(result.compressTimeMs, "ms", 18)
                  << FormatWithUnit(result.decompressTimeMs, "ms", 18)
                  << FormatWithUnit(result.compressionRatio, "%", 10)
                  << std::endl;
    }

    std::cout << std::endl;
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
        else
        {
            std::cerr << LOG_ERROR << "Invalid arguments.\n";
            printUsage();
            return 0;
        }
    }

    std::cout << COLOR_STR("======= [Start analyze!] =======", COLOR_GREEN) << "\n";

    // 1. 加载数组数据
    std::vector<uint32_t> data = LoadArrayFromTxt(argv[FILE_PATH]);

    ArrayData1D inputData1D;
    ArrayData2D inputData2D;
    ArrayData1D outputData1D;
    ArrayData2D outputData2D;
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
        if (ReshapeTo2D(data, inputData2D.rowCount, inputData2D.colCount, inputData2D.arrayData) == SAA_SUCCESS)
        {
            // PrintVector2D(inputData2D.arrayData, inputData2D.rowCount, inputData2D.colCount);
            inputDimension = ARRAY_2D;
        }
        else
        {
            std::cout << LOG_ERROR << "Failed to reshape to 2D array. Please check the input format.\n";
            return 1;
        }
    }
    else
    {
        std::cout << LOG_ERROR << "Invalid dimension argument. Use 1 for 1D or 2 for 2D.\n";
        return 1;
    }

    // 2. 获取所有已注册算法
    const auto &allModes = CompressorRegistry::Instance().ListAlgorithms();

    std::cout << COLOR_STR("==== Compression Comparison Report ====", COLOR_PURPLE) << "\n";
    std::cout << "Input size: " << COLOR_STR(std::to_string(data.size()), COLOR_BLUE) << "elements\n\n";

    ArrayInput input = (inputDimension == ARRAY_1D) ? ArrayInput{inputData1D} : ArrayInput{inputData2D};
    ArrayInput output = (inputDimension == ARRAY_1D) ? ArrayInput{outputData1D} : ArrayInput{outputData2D};
    std::vector<CalResult> results;

    // 3. 遍历每种压缩算法进行测试
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

        ret = compressor->Decompress(output);
        if (ret != SAA_SUCCESS)
        {
            std::cerr << LOG_ERROR << "Decompression failed for " << mode << ". Error code: " << static_cast<int>(ret) << "\n";
            continue;
        }

        CalResult rst;
        if (compressor->GetResult(rst) != SAA_SUCCESS)
        {
            std::cerr << LOG_ERROR << "Failed to get compression result for " << mode << ".\n";
            continue;
        }
        results.push_back(rst);
    }

    // 4. 打印所有结果
    PrintResultTable(results);

    return 0;
}
