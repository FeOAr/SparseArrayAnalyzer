/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-02 20:30:05
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-19 17:09:07
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_run_length.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include "common.h"
#include <chrono>
#include <cmath>

typedef struct rle_node
{
    uint32_t value;  
    uint32_t count;
}RLE_Node;

class RunLengthEnc : public SparseArrayCompressor
{
public:
    int8_t Compress(const ArrayInput &input) override;
    int8_t Decompress(ArrayInput &output) override;
    int8_t GetResult(CalResult &result) const override;

private:
    int8_t startCompress();
    int8_t startDecompress(ArrayData1D &output);

    // Input
    ArrayDimension _arrayType;
    ArrayData1D _inputData1D;
    
    // Output
    CalResult _result;
    std::vector<RLE_Node> _compressedData;
};

int8_t RunLengthEnc::Compress(const ArrayInput &input)
{
    // 1. 解析数据类型
    if(std::holds_alternative<ArrayData1D>(input))
    {
        auto &mat = std::get<ArrayData1D>(input);
        _inputData1D = mat;
        _arrayType = ARRAY_1D;
    }
    else if (std::holds_alternative<ArrayData2D>(input))
    {
        std::cerr << LOG_WARN << "Input data is unsupported dimension.\n";
        return ERROR_UNSUPPORT_DIMENSION;
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
    _result.modeName = "RunLengthEnc";
    _result.originElementCount = GetArrayElemCount1D(_inputData1D.arrayData);
    _result.compressedElementCount = _compressedData.size() * 2; // 每个坐标信息包含2个元素：value, count

    _result.originSizeBytes = GetArrayTotalSize1D(_inputData1D.arrayData);
    _result.compressedSizeBytes = _compressedData.size() * sizeof(RLE_Node);

    _result.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    _result.decompressTimeMs = 0;

    _result.compressionRatio = (static_cast<double>(_result.compressedSizeBytes) / _result.originSizeBytes) * 100.0;

    return SAA_SUCCESS;
}

int8_t RunLengthEnc::startCompress() 
{
#if 1
    uint32_t currentVal = _inputData1D.arrayData[0];
    uint32_t count = 1;

    for (uint32_t i = 1; i < _inputData1D.arrayData.size(); i++)
    {
        if (_inputData1D.arrayData[i] == currentVal)
        {
            ++count;
        }
        else
        {
            _compressedData.push_back({currentVal, count});
            currentVal = _inputData1D.arrayData[i];
            count = 1;
        }
    }

    // The last run
    _compressedData.push_back({currentVal, count});

#if 0
    std::cout << LOG_DEBUG << "CompressedData: \n";
    for (const auto &coord : _compressedData)
    {
        std::cout << "RLE_Node: ("
                  << std::setw(6) << coord.value << ", "
                  << std::setw(4) << coord.count << ")\n";
    }
#endif
 
#endif
    return SAA_SUCCESS;  // 返回值可以根据实际需要调整
}

int8_t RunLengthEnc::Decompress(ArrayInput &output)
{
    // 0. 检查
    if (_inputData1D.arrayData.empty())
    {
        std::cerr << LOG_ERROR << "Input data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    if (_arrayType != ARRAY_1D)
    {
        std::cerr << LOG_ERROR << "ArrayInput is not compatible with 1D array.\n";
        return ERROR_UNSUPPORT_DIMENSION;
    }

    if (_compressedData.empty())
    {
        std::cerr << LOG_ERROR << "Compressed data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    auto *ptr1d = std::get_if<ArrayData1D>(&output);
    if (!ptr1d)
    {
        std::cerr << LOG_ERROR << "ArrayInput is not compatible with 2D array.\n";
        return ERROR_PARAM_INVALID;
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
    *ptr1d = tempData;
    
    _result.decompressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    // 2. 校验
    if(_inputData1D.arrayData != tempData.arrayData)
    {
        std::cerr << LOG_ERROR << "Decompressed result error.\n";
        return ERROR_CALCULATE_ERROR;
    }
    return SAA_SUCCESS;
}

int8_t RunLengthEnc::startDecompress(ArrayData1D &outData1D)
{
    for (const auto &pair : _compressedData)
    {
        outData1D.arrayData.insert(outData1D.arrayData.end(), pair.count, pair.value);
    }

    // PrintVector1D(outData1D.arrayData);
    return SAA_SUCCESS;
}

int8_t RunLengthEnc::GetResult(CalResult &ret) const
{
    ret = _result;
    return SAA_SUCCESS;
}

#if ALGORITHM_RUN_LENGTH
static bool coord_registered = []
{
    CompressorRegistry::Instance().Register("RunLengthEnc", []
                                            { return std::make_unique<RunLengthEnc>(); });
    return true;
}();
#endif