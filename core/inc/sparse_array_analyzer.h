/*
 * @Author: 赵航锐 esnasc@163.com
 * @Date: 2025-07-01 09:17:20
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-06 15:45:27
 * @FilePath: \SparseArrayAnalyzer\core\inc\sparse_array_analyzer.h
 * @Description: 
 */

#ifndef _SPARSE_ARRAY_ANALYZER_H_
#define _SPARSE_ARRAY_ANALYZER_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

/* ----------------------------------- 流程 ----------------------------------- */
#if 0
// 1. 在某处注册算法
CompressorRegistry::Instance().Register("CoordinateCompression", [](){
    return std::make_unique<CoordinateCompression>();
});
CompressorRegistry::Instance().Register("DenseCompression", [](){
    return std::make_unique<DenseCompression>();
});

// 2. 用户选择算法名字，创建实例
auto compressor = CompressorRegistry::Instance().Create("CoordinateCompression");

// 3. 调用压缩
compressor->Compress(inputArray);

// 4. 获取结果
auto result = compressor->GetResult();

// 5. 解压
auto decompressed = compressor->Decompress();
#endif
/* ----------------------------------- end ---------------------------------- */

// 统一分析结果
struct CalResult
{
    //TODO: C++17的结构体成员初始化
    // 名字在map中充当key
    // Array element count
    uint32_t originElementCount = 0;     // 原数组元素数量
    uint32_t compressedElementCount = 0; // 压缩后元素数量

    // Array size in bytes
    uint32_t originSizeBytes = 0;        // 原数组大小
    uint32_t compressedSizeBytes = 0;    // 压缩后数组大小

    // Compression cost
    double compressTimeMs = 0;
    double decompressTimeMs = 0;

    // Compression ratio
    double compressionRatio = 0.0;
};

// 接口定义
class SparseArrayCompressor
{
public:
    virtual ~SparseArrayCompressor() = default;

    // 压缩主入口
    virtual bool Compress(const std::vector<uint32_t> &input) = 0;

    // 解压主入口
    virtual std::vector<uint32_t> Decompress() const = 0;

    // 获取压缩结果
    virtual CalResult GetResult() const = 0;
};

// 工厂注册器（可选）
using CompressorFactory = std::function<std::unique_ptr<SparseArrayCompressor>()>;  //TODO: 记录function用法

class CompressorRegistry
{
public:
    static CompressorRegistry &Instance();  //TODO: 单例模式， static成员函数

    void Register(const std::string &name, CompressorFactory factory);  //TODO: 注册保存function，与算法对象实现分离
    std::unique_ptr<SparseArrayCompressor> Create(const std::string &name) const;  // TODO: 算法对象延迟创建
    std::vector<std::string> ListAlgorithms() const;

private:
    std::unordered_map<std::string, CompressorFactory> _factories;
};

#endif // _SPARSE_ARRAY_ANALYZER_H_