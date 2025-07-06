/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 21:28:22
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-06 18:34:27
 * @FilePath: \SparseArrayAnalyzer\core\src\algorithm_dense.cpp
 * @Description: 
 * 
 */
#include "sparse_array_analyzer.h"
#include <chrono>

class DenseStorage : public SparseArrayCompressor
{
public:
    int8_t Compress(const std::vector<uint32_t> &input) override
    {
        auto start = std::chrono::high_resolution_clock::now();
        _inputData = input;
        auto end = std::chrono::high_resolution_clock::now();

        result_.compressedElementCount = static_cast<uint32_t>(_inputData.size());
        result_.compressedSizeBytes = static_cast<uint32_t>(_inputData.size() * sizeof(uint32_t));
        result_.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        return true;
    }

    int8_t Decompress(std::vector<uint32_t> &input) override
    {
        return 0;
    }

    CalResult GetResult() const override
    {
        return result_;
    }

private:
    std::vector<uint32_t> _inputData;
    CalResult result_;
};

// 注册到工厂（在 main 或注册器中调用）
// TODO：这种机制保证只注册一次
#if 0
static bool dense_registered = []
{
    CompressorRegistry::Instance().Register("DenseArray", []
                                            { return std::make_unique<DenseStorage>(); });
    return true;
}();
#endif
