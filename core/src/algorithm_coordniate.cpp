/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-02 20:30:05
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-06 15:49:35
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_coordniate.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include <chrono>

class CoordinateList : public SparseArrayCompressor
{
public:
    bool Compress(const std::vector<uint32_t> &input) override;
    std::vector<uint32_t> Decompress() const override { return data_; }
    CalResult GetResult() const override { return result_; }

private:
    std::vector<uint32_t> data_;
    CalResult result_;
};

bool CoordinateList::Compress(const std::vector<uint32_t> &input)
{
    auto start = std::chrono::high_resolution_clock::now();
    data_ = input;
    //= work here
    

    auto end = std::chrono::high_resolution_clock::now();

    result_.compressedElementCount = static_cast<uint32_t>(data_.size());
    result_.compressedSizeBytes = static_cast<uint32_t>(data_.size() * sizeof(uint32_t));
    result_.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
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
