/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 21:28:22
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-19 23:36:52
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_dense.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include "common.h"
#include <chrono>

class DenseStorage : public SparseArrayCompressor
{
public:
    int8_t Compress(const ArrayInput &input) override;

    int8_t Decompress(ArrayInput &output) override;

    int8_t GetResult(CalResult &ret) const override;

private:
    ArrayDimension _arrayType;
    ArrayData1D _inputData1D;
    ArrayData2D _inputData2D;
    CalResult _result;
};

int8_t DenseStorage::Compress(const ArrayInput &input)
{
    // 1. 解析数据类型
    if (std::holds_alternative<ArrayData1D>(input))
    {
        auto &vec = std::get<ArrayData1D>(input);
        _inputData1D = vec;
        _arrayType = ARRAY_1D;
    }
    else if (std::holds_alternative<ArrayData2D>(input))
    {
        auto &mat = std::get<ArrayData2D>(input);
        _inputData2D = mat;
        _arrayType = ARRAY_2D;
    }
    else
    {
        std::cerr << LOG_ERROR <<"Input data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    // 2. 计算压缩结果
    _result.modeName = "DenseStorage(origin)";
    _result.originElementCount = (_arrayType == ARRAY_1D)
                                     ? GetArrayElemCount1D(_inputData1D.arrayData)
                                     : GetArrayElemCount2D(_inputData2D.arrayData);
    _result.compressedElementCount = _result.originElementCount;
    _result.originSizeBytes = (_arrayType == ARRAY_1D)
                                  ? GetArrayTotalSize1D(_inputData1D.arrayData)
                                  : GetArrayTotalSize2D(_inputData2D.arrayData);
    _result.compressedSizeBytes = _result.originSizeBytes;
    _result.compressTimeMs = 0;
    _result.decompressTimeMs = 0;
    _result.compressionRatio = 100.0; // 无压缩

    return SAA_SUCCESS;
}

int8_t DenseStorage::Decompress(ArrayInput &output)
{
    if (_arrayType == ARRAY_1D)
    {
        if (auto *ptr1d = std::get_if<ArrayData1D>(&output))
        {
            ptr1d->arrayData = _inputData1D.arrayData;
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
            ptr2d->arrayData = _inputData2D.arrayData;
        }
        else
        {
            std::cerr << LOG_ERROR << "ArrayInput is not compatible with 2D array.\n";
            return ERROR_PARAM_INVALID;
        }
    }

    return SAA_SUCCESS;
}

int8_t DenseStorage::GetResult(CalResult &ret) const
{
    ret = _result;
    return SAA_SUCCESS;
}

#if ALGORITHM_DENSE
static bool dense_registered = []
{
    CompressorRegistry::Instance().Register("DenseArray", []
                                            { return std::make_unique<DenseStorage>(); });
    return true;
}();
#endif
