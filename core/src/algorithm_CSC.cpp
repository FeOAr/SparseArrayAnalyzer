/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-18 19:16:11
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-19 15:04:53
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_CSC.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include "common.h"
#include <chrono>
#include <cmath>

typedef struct csc_compressed_indo
{
    std::vector<uint32_t> values;
    std::vector<uint32_t> rowInd;
    std::vector<uint32_t> colOffset;
    uint32_t mainValue;
    uint32_t rows;
    uint32_t cols;
} CscCompressed;

class CompressedSparseCol : public SparseArrayCompressor
{
public:
    int8_t Compress(const ArrayInput &input) override;
    int8_t Decompress(ArrayInput &output) override;
    int8_t GetResult(CalResult &result) const override;

private:
    int8_t startCompress();
    int8_t startDecompress(ArrayData2D &output);

    // Input
    ArrayDimension _arrayType;
    ArrayData2D _inputData2D;

    // Output
    CscCompressed _compressedData;
    CalResult _result;
};

int8_t CompressedSparseCol::Compress(const ArrayInput &input)
{
    // 1. 解析数据类型
    if (std::holds_alternative<ArrayData1D>(input))
    {
        std::cerr << LOG_WARN << "Input data is unsupported dimension.\n";
        return ERROR_UNSUPPORT_DIMENSION;
    }
    else if (std::holds_alternative<ArrayData2D>(input))
    {
        auto &mat = std::get<ArrayData2D>(input);
        _inputData2D = mat;
        _arrayType = ARRAY_2D;
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
    _result.modeName = "CompressedSparseCol";
    _result.originElementCount = GetArrayElemCount2D(_inputData2D.arrayData);
    _result.compressedElementCount = _compressedData.values.size() + _compressedData.rowInd.size() + _compressedData.colOffset.size(); // 每个坐标信息包含3个元素：x_coord, y_coord, value

    _result.originSizeBytes = GetArrayTotalSize2D(_inputData2D.arrayData);
    _result.compressedSizeBytes = GetArrayTotalSize1D(_compressedData.values) +
                                  GetArrayTotalSize1D(_compressedData.rowInd) +
                                  GetArrayTotalSize1D(_compressedData.colOffset) +
                                  3 * sizeof(uint32_t);

    _result.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    _result.decompressTimeMs = 0;

    _result.compressionRatio = (static_cast<double>(_result.compressedSizeBytes) / _result.originSizeBytes) * 100.0;

    return SAA_SUCCESS;
}

int8_t CompressedSparseCol::startCompress()
{
#if 1
    uint32_t row = _inputData2D.rowCount;
    uint32_t col = _inputData2D.colCount;

    // 0. 分析数组大小，合理调整成二维结构
    // uint32_t arrayCount = static_cast<uint32_t>(_inputData.size());
    // uint32_t tempCol = std::sqrt(arrayCount);
    // uint32_t tempRow = (arrayCount + tempCol - 1) / tempCol;  // TODO：注意这种向上取整方法

    // 1. 分析数组，统计各个值的出现次数
    std::unordered_map<uint32_t, uint32_t> valueCount;
    for (const auto &row : _inputData2D.arrayData)
    {
        for (const auto &val : row)
        {
            valueCount[val]++;
        }
    }

    uint32_t mainVal = 0;               // 主值
    uint32_t mainValCount = 0;          // 主值出现次数
    for (const auto &pair : valueCount) // TODO: 记录一下容器遍历的常用办法以及优缺点
    {
        if (pair.second > mainValCount)
        {
            mainVal = pair.first;
            mainValCount = pair.second;
        }
    }

    _compressedData.mainValue = mainVal; // 记录原始信息
    _compressedData.rows = row;
    _compressedData.cols = col;

    // std::cout << LOG_DEBUG << "Main value: " << mainVal << ", Count: " << mainValCount << "\n";

    // 2. 根据主值进行坐标法压缩
    uint32_t count = 0;
    _compressedData.colOffset.push_back(0);
    for (uint32_t i = 0; i < col; i++)
    {
        for (uint32_t n = 0; n < row; n++)
        {
            if (_inputData2D.arrayData[n][i] != mainVal)
            {
                _compressedData.values.push_back(_inputData2D.arrayData[n][i]);
                _compressedData.rowInd.push_back(n);
                ++count;
            }
        }
        _compressedData.colOffset.push_back(count);
    }

#if 0
    std::cout << LOG_DEBUG << "CompressedData: \n";
    PrintVector1D(_compressedData.colOffset, "Col Offset");
    PrintVector1D(_compressedData.rowInd, "Row Indices");
    PrintVector1D(_compressedData.values, "Values");
    std::cout << LOG_DEBUG << "mainValue: " << _compressedData.mainValue << "\n";
#endif

#endif
    return SAA_SUCCESS; // 返回值可以根据实际需要调整
}

int8_t CompressedSparseCol::Decompress(ArrayInput &output)
{
    // 0. 检查
    if (_inputData2D.arrayData.empty())
    {
        std::cerr << LOG_ERROR << "Input data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    if (_arrayType != ARRAY_2D)
    {
        std::cerr << LOG_ERROR << "ArrayInput is not compatible with 2D array.\n";
        return ERROR_UNSUPPORT_DIMENSION;
    }

    if (_compressedData.colOffset.empty() || _compressedData.rowInd.empty() || _compressedData.values.empty())
    {
        std::cerr << LOG_ERROR << "Compressed data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    auto *ptr2d = std::get_if<ArrayData2D>(&output);
    if (!ptr2d)
    {
        std::cerr << LOG_ERROR << "ArrayInput is not compatible with 2D array.\n";
        return ERROR_PARAM_INVALID;
    }

    // 1. 解压
    ArrayData2D tempData;
    auto start = std::chrono::high_resolution_clock::now();
    if (startDecompress(tempData) != SAA_SUCCESS)
    {
        std::cerr << LOG_ERROR << "Decompressed something error.\n";
        return ERROR_CALCULATE_ERROR;
    }
    auto end = std::chrono::high_resolution_clock::now();
    *ptr2d = tempData;

    _result.decompressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();

    // 2. 校验
    if (!Compare2D(_inputData2D.arrayData, tempData.arrayData))
    {
        std::cerr << LOG_ERROR << "Decompressed result error.\n";
        return ERROR_CALCULATE_ERROR;
    }
    return SAA_SUCCESS;
}

int8_t CompressedSparseCol::startDecompress(ArrayData2D &outData2D)
{
    // 1. 填充主值
    outData2D.arrayData.resize(_compressedData.rows, std::vector<uint32_t>(_compressedData.cols, _compressedData.mainValue));

    outData2D.rowCount = _compressedData.rows;
    outData2D.colCount = _compressedData.cols;

    // 2. 填充非主值（列主解压）
    for (uint32_t j = 0; j < outData2D.colCount; ++j)
    {
        for (uint32_t idx = _compressedData.colOffset[j]; idx < _compressedData.colOffset[j + 1]; ++idx)
        {
            uint32_t i = _compressedData.rowInd[idx];
            outData2D.arrayData[i][j] = _compressedData.values[idx];
        }
    }

    // PrintVector2D(outData2D.arrayData, outData2D.rowCount, outData2D.colCount);
    return SAA_SUCCESS;
}

int8_t CompressedSparseCol::GetResult(CalResult &ret) const
{
    ret = _result;
    return SAA_SUCCESS;
}

#if ALGORITHM_CSC
static bool coord_registered = []
{
    CompressorRegistry::Instance().Register("CSC", []
                                            { return std::make_unique<CompressedSparseCol>(); });
    return true;
}();
#endif
