/*
 * @Author: 赵航锐 esnasc@163.com
 * @Date: 2025-07-01 09:17:20
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-02 20:43:44
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

// 统一分析结果
struct CalResult
{
    std::string algorithmName;
    uint32_t compressedElementCount = 0;
    uint32_t compressedSizeBytes = 0;
    double compressTimeMs = 0;
    double decompressTimeMs = 0;
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
    std::unordered_map<std::string, CompressorFactory> factories_;
};

#endif // _SPARSE_ARRAY_ANALYZER_H_