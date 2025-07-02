/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 21:28:22
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-02 22:34:41
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_dense.cpp
 * @Description: 
 * 
 */
#include "sparse_array_analyzer.h"
#include <chrono>

class DenseStorage : public SparseArrayCompressor
{
public:
    bool Compress(const std::vector<uint32_t> &input) override
    {
        auto start = std::chrono::high_resolution_clock::now();
        data_ = input;
        auto end = std::chrono::high_resolution_clock::now();

        result_.algorithmName = "Dense";
        result_.compressedElementCount = static_cast<uint32_t>(data_.size());
        result_.compressedSizeBytes = static_cast<uint32_t>(data_.size() * sizeof(uint32_t));
        result_.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        return true;
    }

    std::vector<uint32_t> Decompress() const override
    {
        return data_;
    }

    CalResult GetResult() const override
    {
        return result_;
    }

private:
    std::vector<uint32_t> data_;
    CalResult result_;
};

// 注册到工厂（在 main 或注册器中调用）
// TODO：这种机制保证只注册一次
static bool dense_registered = []
{
    CompressorRegistry::Instance().Register("DenseArray", []
                                            { return std::make_unique<DenseStorage>(); });
    return true;
}();
