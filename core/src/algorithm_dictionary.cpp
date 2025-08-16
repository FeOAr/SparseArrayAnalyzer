/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-18 09:56:29
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-08-16 10:17:34
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_dictionary.cpp
 * @Description:
 *
 */

#include "sparse_array_analyzer.h"
#include "common.h"
#include <chrono>
#include <cmath>
#include "tool.hpp"

typedef struct compress_dict
{
    std::vector<uint32_t> valueDict;
    std::vector<uint8_t> indexBitTable;
    uint8_t bitWidth; // index 位宽
    uint32_t originCount;
    uint32_t originArrayRow;
    uint32_t originArrayCol;
} CompressDict;

class DictionaryEnc : public SparseArrayCompressor
{
    int8_t Compress(const ArrayInput &input) override;
    int8_t Decompress(ArrayInput &output) override;
    int8_t GetResult(CalResult &result) const override;

private:
    int8_t startCompress();
    int8_t startDecompress(ArrayData1D &output);
    void PrintBitPackedIndices(const std::vector<uint8_t> &vec, uint8_t bitWidth, uint8_t indicesPerLine = 16);

    // Input
    ArrayData1D _inputData1D;
    ArrayDimension _arrayType;

    // Output
    CompressDict _compressedData;
    CalResult _result;
};

int8_t DictionaryEnc::Compress(const ArrayInput &input)
{
    // 1. 预处理输入数据
    if (std::holds_alternative<ArrayData1D>(input))
    {
        const auto &vec = std::get<ArrayData1D>(input);
        _inputData1D = vec;
        _arrayType = ARRAY_1D;
    }
    else if (std::holds_alternative<ArrayData2D>(input))
    {
        _arrayType = ARRAY_2D;
        const auto &vec2d = std::get<ArrayData2D>(input);

        _compressedData.originArrayRow = vec2d.rowCount;
        _compressedData.originArrayCol = vec2d.colCount;

        std::vector<uint32_t> flat;
        flat.reserve(_compressedData.originArrayRow * _compressedData.originArrayCol);
        for (const auto &row : vec2d.arrayData)
        {
            flat.insert(flat.end(), row.begin(), row.end());
        }

        _inputData1D.arrayData = std::move(flat);
    }
    else
    {
        std::cerr << LOG_ERROR << "Input data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    auto start = std::chrono::high_resolution_clock::now();
    startCompress();
    auto end = std::chrono::high_resolution_clock::now();

    // 2. 计算压缩结果
    _result.modeName = "HashDictionary";
    _result.originElementCount = GetArrayElemCount1D(_inputData1D.arrayData);
    _result.compressedElementCount = _compressedData.valueDict.size() + _compressedData.indexBitTable.size() + 4;

    _result.originSizeBytes = GetArrayTotalSize1D(_inputData1D.arrayData);
    _result.compressedSizeBytes = _compressedData.indexBitTable.size() + GetArrayTotalSize1D(_compressedData.valueDict) + 3 * sizeof(uint32_t) + 1;

    _result.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    _result.decompressTimeMs = 0;

    _result.compressionRatio = (static_cast<double>(_result.compressedSizeBytes) / _result.originSizeBytes) * 100.0;

    return SAA_SUCCESS;
}

int8_t DictionaryEnc::startCompress()
{
#if 1
    std::unordered_map<uint32_t, uint32_t> dictMap;
    std::vector<uint32_t> tempIndexTable;

    _compressedData.originCount = static_cast<uint32_t>(_inputData1D.arrayData.size());
    _compressedData.valueDict.reserve(_inputData1D.arrayData.size());
    tempIndexTable.reserve(_inputData1D.arrayData.size());

    // 1. 数组取值，存储去重
    for (const uint32_t &val : _inputData1D.arrayData)
    {
        auto it = dictMap.find(val);
        if (it == dictMap.end())
        {
            uint32_t newIndex = dictMap.size();
            dictMap[val] = newIndex;
            _compressedData.valueDict.push_back(val);
            tempIndexTable.push_back(newIndex);
        }
        else
        {
            tempIndexTable.push_back(it->second);
        }
    }

    // 2. 压缩索引
    uint8_t bitWidth = static_cast<uint8_t>(std::ceil(std::log2(dictMap.size())));
    std::vector<uint8_t> packedBits;
    packedBits.reserve((tempIndexTable.size() * bitWidth + 7) / 8); // 精确字节数，也进行了向上取整

    uint8_t currentByte = 0;
    uint8_t bitPos = 0;

    for (uint32_t idx : tempIndexTable)
    {
        for (int8_t i = bitWidth - 1; i >= 0; --i) // 从高位到低位
        {
            uint8_t bit = (idx >> i) & 1;
            currentByte = (currentByte << 1) | bit;
            ++bitPos;

            if (bitPos == 8)
            {
                packedBits.push_back(currentByte);
                currentByte = 0;
                bitPos = 0;
            }
        }
    }

    // 最后不足8位的补全
    if (bitPos > 0)
    {
        currentByte <<= (8 - bitPos); // 左移补零，补在低位
        packedBits.push_back(currentByte);
    }
    _compressedData.indexBitTable = std::move(packedBits);
    _compressedData.bitWidth = bitWidth;

    // PrintBuffer(_compressedData.indexBitTable);

#if 0
    std::cout << LOG_DEBUG << "Hash Dictionary info: (originCount: " << _compressedData.originCount
              << " originArrayRow: " << _compressedData.originArrayRow
              << " originArrayCol: " << _compressedData.originArrayCol
              << " bitWidth: " << static_cast<int>(_compressedData.bitWidth)
              << ")\n";

    PrintVector1D(_compressedData.valueDict, "_compressedData.valueDict");
    PrintBitPackedIndices(_compressedData.indexBitTable, _compressedData.bitWidth);

#endif

#endif
    return SAA_SUCCESS;
}

int8_t DictionaryEnc::Decompress(ArrayInput &output)
{
    if (_inputData1D.arrayData.empty())
    {
        std::cerr << LOG_ERROR << "Input data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    if (_compressedData.valueDict.empty() && _compressedData.indexBitTable.empty())
    {
        std::cerr << LOG_ERROR << "Compressed data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    // 1. 解压
    ArrayData1D tempData;
    auto start = std::chrono::high_resolution_clock::now();
    if (startDecompress(tempData) != SAA_SUCCESS)
    {
        std::cerr << LOG_ERROR << "Decompressed something error.\n";
        return ERROR_CALCULATE_ERROR;
    }
    auto end = std::chrono::high_resolution_clock::now();

    _result.decompressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    // 2. 校验
    if (_inputData1D.arrayData != tempData.arrayData)
    {
        std::cerr << LOG_ERROR << "Decompressed result error.\n";
        return ERROR_CALCULATE_ERROR;
    }
    
    // 3. 维度恢复
    if (_arrayType == ARRAY_1D)
    {
        // PrintVector1D(tempData.arrayData, "Decompressed Array 1D");

        if (auto *ptr1d = std::get_if<ArrayData1D>(&output))
        {
            *ptr1d = std::move(tempData);
        }
        else
        {
            std::cerr << LOG_ERROR << "ArrayInput is not compatible with 1D array.\n";
            return ERROR_PARAM_INVALID;
        }
    }
    else if (_arrayType == ARRAY_2D)
    {
        if (auto *ptr2d = std::get_if<ArrayData2D>(&output))
        {
            ArrayData2D out;
            out.arrayData.resize(_compressedData.originArrayRow);
            out.rowCount = _compressedData.originArrayRow;
            out.colCount = _compressedData.originArrayCol;
            for (uint32_t r = 0; r < _compressedData.originArrayRow; ++r)
            {
                out.arrayData[r].assign(
                    tempData.arrayData.begin() + r * _compressedData.originArrayCol,
                    tempData.arrayData.begin() + (r + 1) * _compressedData.originArrayCol);
            }
            // PrintVector2D(out.arrayData, out.rowCount, out.colCount, "Decompressed Array 2D");
            *ptr2d = std::move(out);
        }
        else
        {
            std::cerr << LOG_ERROR << "ArrayInput is not compatible with 2D array.\n";
            return ERROR_PARAM_INVALID;
        }
    }

    return SAA_SUCCESS;
}

int8_t DictionaryEnc::startDecompress(ArrayData1D &outData1D)
{
    const std::vector<uint8_t> &packed = _compressedData.indexBitTable;
    const uint8_t bitWidth = _compressedData.bitWidth;
    const uint32_t indexCount = _compressedData.originCount;

    outData1D.arrayData.reserve(indexCount);

    size_t bitPos = 0;
    for (size_t i = 0; i < indexCount; ++i)
    {
        uint32_t idx = 0;

        // 大端位序解码（高位在前）
        for (uint8_t b = 0; b < bitWidth; ++b)
        {
            size_t byteIndex = (bitPos + b) / 8;
            size_t bitOffset = 7 - ((bitPos + b) % 8); // 大端：高位优先

            if (byteIndex >= packed.size())
                return ERROR_INDEX_OUT_OF_RANGE;

            uint8_t bit = (packed[byteIndex] >> bitOffset) & 1;
            idx = (idx << 1) | bit;
        }

        if (idx >= _compressedData.valueDict.size())
            return ERROR_INDEX_OUT_OF_RANGE;

        outData1D.arrayData.push_back(_compressedData.valueDict[idx]);

        bitPos += bitWidth;
    }

    return SAA_SUCCESS;
}

int8_t DictionaryEnc::GetResult(CalResult &ret) const
{
    ret = _result;
    return SAA_SUCCESS;
}

void DictionaryEnc::PrintBitPackedIndices(const std::vector<uint8_t> &vec, uint8_t bitWidth, uint8_t indicesPerLine)
{
    std::cout << "Bit-packed indices (" << vec.size() << " entries, " << int(bitWidth) << " bits each, big-endian):\n";

    uint32_t bitPos = 0;
    for (uint32_t i = 0; i < vec.size(); ++i)
    {
        uint32_t value = 0;

        for (uint32_t b = 0; b < bitWidth; ++b)
        {
            uint32_t bitIndex = bitPos + b;
            uint32_t byteIndex = bitIndex / 8;
            uint32_t bitOffset = 7 - (bitIndex % 8); // 高位在前（大端）

            uint8_t bit = (vec[byteIndex] >> bitOffset) & 1;
            std::cout << (bit ? '1' : '0');

            value = (value << 1) | bit; // 从高位拼接
        }

        std::cout << " ";

        if ((i + 1) % indicesPerLine == 0)
            std::cout << '\n';
        bitPos += bitWidth;
    }

    std::cout << '\n';
}

#if ALGORITHM_DICTIONARY
static bool coord_registered = []
{
    CompressorRegistry::Instance().Register("HashDictionary", []
                                            { return std::make_unique<DictionaryEnc>(); });
    return true;
}();
#endif
