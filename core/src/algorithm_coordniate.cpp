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
    uint32_t x_coord;  // 本例中行列从1开始，(0,0)记录主值
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
    int8_t startCompress(const std::vector<uint32_t> &input);
    int8_t startDecompress(std::vector<uint32_t> &output) const;  // TODO: const这一块不太懂
    std::vector<uint32_t> _inputData;
    std::vector<CoordInfo> _compressedData;
    CalResult result_;
};

int8_t CoordinateList::Compress(const std::vector<uint32_t> &input)
{
    if (input.empty())
    {
        return ERROR_INPUT_EMPTY;
    }

    auto start = std::chrono::high_resolution_clock::now();
    startCompress(input);
    auto end = std::chrono::high_resolution_clock::now();

    result_.compressedElementCount = static_cast<uint32_t>(_inputData.size());
    result_.compressedSizeBytes = static_cast<uint32_t>(_inputData.size() * sizeof(uint32_t));
    result_.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    return true;
}

int8_t CoordinateList::startCompress(const std::vector<uint32_t> &input)
{
    _inputData = input;

    // 0. 分析数组大小，合理调整成二维结构
    uint32_t arrayCount = static_cast<uint32_t>(_inputData.size());
    uint32_t tempCol = std::sqrt(arrayCount);
    uint32_t tempRow = (arrayCount + tempCol - 1) / tempCol;  // TODO：注意这种向上取整方法

    // 1. 分析数组，统计各个值的出现次数
    std::unordered_map<uint32_t, uint32_t> valueCount;
    for (const auto &value : _inputData)
    {
        valueCount[value]++;
    }

    // 2. 根据主值进行坐标法压缩

    
    result_.originElementCount = static_cast<uint32_t>(input.size());
    result_.originSizeBytes = static_cast<uint32_t>(input.size() * sizeof(uint32_t));
    result_.compressionRatio = static_cast<double>(result_.originSizeBytes) / result_.compressedSizeBytes;
    return 0;  // 返回值可以根据实际需要调整
}

int8_t CoordinateList::Decompress(std::vector<uint32_t> &input)
{
    if (input.empty())
    {
        return ERROR_INPUT_EMPTY;
    }

    return true;
}

// 注册到工厂（在 main 或注册器中调用）
// TODO：这种机制保证只注册一次
static bool coord_registered = []
{
    CompressorRegistry::Instance().Register("CoordinateList", []
                                            { return std::make_unique<CoordinateList>(); });
    return true;
}();
