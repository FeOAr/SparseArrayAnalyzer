/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-06-30 21:23:29
 * @FilePath: \SparseArrayAnalyzer\test\main.cpp
 * @Description:
 *
 */
#include "SparseArrayCompressor.h"
#include "CompressorRegistry.h"
#include <iostream>
#include <memory>
#include <vector>

int main()
{
    // 假设原始数组
    std::vector<uint32_t> data = {
        0, 0, 128, 0, 0, 999, 0, 0, 1024, 0, 0, 0, 0, 8888};

    // 构造参数
    SparseArrayCompressor::Params params;
    params.dataPtr = reinterpret_cast<const uint8_t *>(data.data());
    params.dataSize = static_cast<uint32_t>(data.size() * sizeof(uint32_t));
    params.elementSize = sizeof(uint32_t);
    params.typeName = "uint32_t";

    // 可选：切换不同算法名（必须在注册表中）
    std::string mode = "dense"; // dense, coordinate, csr, hash_map...

    auto compressor = CompressorRegistry::create(mode);
    if (!compressor)
    {
        std::cerr << "Compressor mode [" << mode << "] not registered.\n";
        return 1;
    }

    CalResult result;
    int32_t ret = compressor->compress(params, &result);
    if (ret != 0)
    {
        std::cerr << "Compression failed: " << result.errorMsg << "\n";
        return 1;
    }

    std::cout << "Compression complete.\n";
    std::cout << "Original size: " << result.originalBytes << " bytes\n";
    std::cout << "Compressed size: " << result.compressedBytes << " bytes\n";
    std::cout << "Compression ratio: " << result.compressionRatio << "\n";
    std::cout << "Time taken: " << result.elapsedUs << " us\n";

    return 0;
}
