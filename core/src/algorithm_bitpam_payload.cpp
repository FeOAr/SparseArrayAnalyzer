/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-02 20:30:05
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-08-16 10:28:02
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_bitpam_payload.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include "common.h"
#include <chrono>
#include <cmath>

typedef struct bitmap_enc_out
{
    uint32_t bitNum;
    uint32_t mainValue;
    uint32_t rows;
    uint32_t cols;
    std::vector<uint8_t> bitmap;
    std::vector<uint32_t> valueTable;
} BitMapCompressed1D;

class BitmapPayloadEnc : public SparseArrayCompressor
{
    int8_t Compress(const ArrayInput &input) override;
    int8_t Decompress(ArrayInput &output) override;
    int8_t GetResult(CalResult &result) const override;

private:
    int8_t startCompress();
    int8_t startDecompress(ArrayData1D &output);

    // Input
    ArrayData1D _inputData1D;
    ArrayDimension _arrayType;

    // Output
    BitMapCompressed1D _compressedData;
    CalResult _result;
};

int8_t BitmapPayloadEnc::Compress(const ArrayInput &input)
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
                
        _compressedData.rows = vec2d.rowCount;
        _compressedData.cols = vec2d.colCount;

        std::vector<uint32_t> flat;
        flat.reserve(_compressedData.rows * _compressedData.cols);
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
    _result.modeName = "BitmapPayload";
    _result.originElementCount = GetArrayElemCount1D(_inputData1D.arrayData);
    _result.compressedElementCount = _compressedData.bitmap.size() + _compressedData.valueTable.size() + 4;

    _result.originSizeBytes = GetArrayTotalSize1D(_inputData1D.arrayData);
    _result.compressedSizeBytes = _compressedData.bitmap.size() + GetArrayTotalSize1D(_compressedData.valueTable) + 4 * sizeof(uint32_t);

    _result.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    _result.decompressTimeMs = 0;

    _result.compressionRatio = (static_cast<double>(_result.compressedSizeBytes) / _result.originSizeBytes) * 100.0;

    return SAA_SUCCESS;
}

int8_t BitmapPayloadEnc::startCompress()
{
#if 1
    // 1. 分析数组，统计各个值的出现次数
    std::unordered_map<uint32_t, uint32_t> valueCount;
    for (const auto &val : _inputData1D.arrayData)
    {
        valueCount[val]++;
    }

    _compressedData.mainValue = 0;
    uint32_t mainValueCount = 0;
    for (const auto &pair : valueCount)
    {
        if (pair.second > mainValueCount)
        {
            _compressedData.mainValue = pair.first;
            mainValueCount = pair.second;
        }
    }

    // std::cout << LOG_DEBUG << "Main value: " << _compressedData.mainValue << ", Count: " << mainValueCount << "\n";

    // 2. 根据主值进行压缩
    _compressedData.bitNum = _inputData1D.arrayData.size();
    uint32_t numBytes = (_compressedData.bitNum + 8 - 1) / 8;
    _compressedData.bitmap.resize(numBytes, 0);

    for (uint32_t i = 0; i < _compressedData.bitNum; i++)
    {
        if (_inputData1D.arrayData[i] != _compressedData.mainValue)
        {
            uint32_t byteIndex = i / 8;
            uint8_t bitOffset = i % 8;
            _compressedData.bitmap[byteIndex] |= (1 << bitOffset);
            _compressedData.valueTable.push_back(_inputData1D.arrayData[i]);
        }
    }

#if 0
    std::cout << LOG_DEBUG << "CompressedData: \n";
    std::cout << LOG_DEBUG << "Bitmap+Payload info: ( " << _compressedData.bitmap.size() << ", " << _compressedData.valueTable.size() << " )\n";

    std::cout << "Bitmap:\n";
    for (const auto &val : _compressedData.bitmap)
    {
        std::cout << static_cast<int>(val) << ",";
    }
    std::cout << "\n";

    std::cout << "valueTab:\n";
    for (const auto &val : _compressedData.valueTable)
    {
        std::cout << static_cast<int>(val) << ", ";
    }
    std::cout << "\n";
#endif

#endif
    return SAA_SUCCESS; // 返回值可以根据实际需要调整
}

int8_t BitmapPayloadEnc::Decompress(ArrayInput &output)
{
    // 0. 检查
    if (_inputData1D.arrayData.empty())
    {
        std::cerr << LOG_ERROR << "Input data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    if (_compressedData.bitmap.empty() && _compressedData.valueTable.empty())
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

    // 3. 维度复原
    if (_arrayType == ARRAY_1D)
    {
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
            out.arrayData.resize(_compressedData.rows);
            for (uint32_t r = 0; r < _compressedData.rows; ++r)
            {
                out.arrayData[r].assign(
                    tempData.arrayData.begin() + r * _compressedData.cols,
                    tempData.arrayData.begin() + (r + 1) * _compressedData.cols);
            }
            out.rowCount = _compressedData.rows;
            out.colCount = _compressedData.cols;
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

int8_t BitmapPayloadEnc::startDecompress(ArrayData1D &outData1D)
{
    uint32_t valIndex = 0;

    // std::cout << LOG_DEBUG << "bitNum: " << _compressedData.bitNum << " bitmap: " << _compressedData.bitmap.size() << "\n";

    for (uint32_t i = 0; i < _compressedData.bitNum; i++)
    {
        uint32_t byteIndex = i / 8;
        uint8_t bitOffset = i % 8;

        bool bitSet = (_compressedData.bitmap[byteIndex] >> bitOffset) & 1;
        if (bitSet)
        {
            outData1D.arrayData.push_back(_compressedData.valueTable[valIndex++]);
        }
        else
        {
            outData1D.arrayData.push_back(_compressedData.mainValue);
        }
    }

    // PrintVector1D(outData1D.arrayData);
    return SAA_SUCCESS;
}

int8_t BitmapPayloadEnc::GetResult(CalResult &ret) const
{
    ret = _result;
    return SAA_SUCCESS;
}

#if ALGORITHM_BITMAP_PAYLOAD
static bool coord_registered = []
{
    CompressorRegistry::Instance().Register("BitmapPayloadEnc", []
                                            { return std::make_unique<BitmapPayloadEnc>(); });
    return true;
}();
#endif