/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-02 20:30:05
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-09 21:18:43
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_coordniate.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include "common.h"
#include <chrono>
#include <cmath>

typedef struct coord_info
{
    //~ 本例中非主值从行列1开始，(0,0)记录规模和主值
    uint32_t x_coord;  
    uint32_t y_coord;
    uint32_t value;
}CoordInfo;

class CoordinateList : public SparseArrayCompressor
{
public:
    int8_t Compress(const ArrayInput &input) override;
    int8_t Decompress(ArrayInput &input) override;
    int8_t GetResult(CalResult &result) const override;

private:
    int8_t startCompress();
    int8_t startDecompress(ArrayData2D &output);  // TODO: const这一块不太懂

    // Input
    ArrayDimension _arrayType;
    ArrayData2D _inputData2D;
    
    // Output
    CalResult _result;
    std::vector<CoordInfo> _compressedData;
};

int8_t CoordinateList::Compress(const ArrayInput &input)
{
    // 1. 解析数据类型
    if(std::holds_alternative<ArrayData1D>(input))
    {
        std::cerr << LOG_ERROR << "Input data is unsupported dimension.\n";
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
    _result.originElementCount = GetArrayElemCount2D(_inputData2D.arrayData);
    _result.compressedElementCount = _compressedData.size() * 3; // 每个坐标信息包含3个元素：x_coord, y_coord, value

    _result.originSizeBytes = GetArrayTotalSize2D(_inputData2D.arrayData);
    _result.compressedSizeBytes = _compressedData.size() * sizeof(CoordInfo);

    _result.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    _result.decompressTimeMs = 0;

    _result.compressionRatio = (static_cast<double>(_result.compressedSizeBytes) / _result.originSizeBytes) * 100.0;

    return SAA_SUCCESS;
}

int8_t CoordinateList::startCompress() 
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

    uint32_t mainValue = 0;  // 主值
    uint32_t mainValueCount = 0;  // 主值出现次数
    for (const auto &pair : valueCount) //TODO: 记录一下容器遍历的常用办法以及优缺点
    {
        if (pair.second > mainValueCount)
        {
            mainValue = pair.first;
            mainValueCount = pair.second;
        }
    }

    _compressedData.push_back({row, col, mainValue});  // 记录原始信息

    std::cout << LOG_DEBUG << "Main value: " << mainValue << ", Count: " << mainValueCount << "\n";

    // 2. 根据主值进行坐标法压缩
    for (uint32_t i = 0; i < row; i++)
    {
        for (uint32_t n = 0; n < col; n++)
        {
            if(_inputData2D.arrayData[i][n] != mainValue)
            {
                CoordInfo coord;
                coord.x_coord =i + 1;       // 行号从1开始
                coord.y_coord = n + 1;      // 列号从1开始
                coord.value = _inputData2D.arrayData[i][n];
                _compressedData.push_back(coord);
            }
        }
    }
# if 0
    std::cout << LOG_DEBUG << "CompressedData: \n";
    for (const auto &coord : _compressedData)
    {
        std::cout << "Coord: (" << coord.x_coord << ", " << coord.y_coord << ") Value: " << coord.value << "\n";
    }
#endif
 
#endif
    return SAA_SUCCESS;  // 返回值可以根据实际需要调整
}

int8_t CoordinateList::Decompress(ArrayInput &input)
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

    if (_compressedData.empty())
    {
        std::cerr << LOG_ERROR << "Compressed data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    auto *ptr2d = std::get_if<ArrayData2D>(&input);
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
    if(!Compare2D(_inputData2D.arrayData, tempData.arrayData))
    {
        std::cerr << LOG_ERROR << "Decompressed result error.\n";
        return ERROR_CALCULATE_ERROR;
    }
    return SAA_SUCCESS;
}

int8_t CoordinateList::startDecompress(ArrayData2D &outData2D)
{
    // 1. 填充主值
    _compressedData[0].x_coord = std::max(_compressedData[0].x_coord, 1u);
    _compressedData[0].y_coord = std::max(_compressedData[0].y_coord, 1u);
    outData2D.arrayData.resize(_compressedData[0].x_coord, std::vector<uint32_t>(_compressedData[0].y_coord, _compressedData[0].value));

    outData2D.rowCount = _compressedData[0].x_coord;
    outData2D.colCount = _compressedData[0].y_coord;

    // 2. 填充非主值
    for(auto valIt = _compressedData.begin(); valIt !=  _compressedData.end(); valIt++)
    {
        outData2D.arrayData[valIt->x_coord - 1][valIt->y_coord - 1] = valIt->value;
    }

    // PrintVector2D(outData2D.arrayData, outData2D.rowCount, outData2D.colCount);
    return SAA_SUCCESS;
}


int8_t CoordinateList::GetResult(CalResult &ret) const
{
    ret = _result;
    return SAA_SUCCESS;
}

// 注册到工厂（在 main 或注册器中调用）
// TODO：这种机制保证只注册一次
static bool coord_registered = []
{
    CompressorRegistry::Instance().Register("CoordinateList", []
                                            { return std::make_unique<CoordinateList>(); });
    return true;
}();
