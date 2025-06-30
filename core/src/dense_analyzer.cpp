#include "sparse_array_analyzer.h"
#include <chrono>

class DenseStorage : public SparseArrayCompressor {
public:
    bool Compress(const std::vector<uint32_t>& input) override {
        auto start = std::chrono::high_resolution_clock::now();
        data_ = input;
        auto end = std::chrono::high_resolution_clock::now();
        
        result_.methodName = "Dense";
        result_.compressedElementCount = static_cast<uint32_t>(data_.size());
        result_.compressedSizeBytes = static_cast<uint32_t>(data_.size() * sizeof(uint32_t));
        result_.compressTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        return true;
    }

    std::vector<uint32_t> Decompress() const override {
        return data_;
    }

    CalResult GetResult() const override {
        return result_;
    }

private:
    std::vector<uint32_t> data_;
    CalResult result_;
};

// 注册到工厂（在 main 或注册器中调用）
static bool dense_registered = [] {
    CompressorRegistry::Instance().Register("Dense", [] {
        return std::make_unique<DenseStorage>();
    });
    return true;
}();
