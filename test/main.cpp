/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-06 15:54:28
 * @FilePath: \SparseArrayAnalyzer\test\main.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include <iostream>
#include <memory>
#include <vector>
#include "common.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <array_input.txt>\n";
        return 1;
    }
    printf("Start analyze!\n");

    // 原始数组
    std::vector<uint32_t> data = loadArrayFromTxt("test\\input_array.txt");

    printVector(data);

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
