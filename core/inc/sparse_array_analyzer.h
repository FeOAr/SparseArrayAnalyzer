/*
 * @Author: 赵航锐 esnasc@163.com
 * @Date: 2025-07-01 09:17:20
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-19 17:07:37
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
#include <variant>
#include <iomanip>

struct ArrayData1D
{
    std::vector<uint32_t> arrayData;
};

struct ArrayData2D
{
    uint32_t rowCount = 0;
    uint32_t colCount = 0;
    std::vector<std::vector<uint32_t>> arrayData;
};

using ArrayInput = std::variant<ArrayData1D, ArrayData2D>;

// 统一分析结果
typedef struct cal_result
{
    // TODO: C++17的结构体成员初始化
    std::string modeName; // 压缩算法名称
    //  Array element count
    uint32_t originElementCount = 0;     // 原数组元素数量
    uint32_t compressedElementCount = 0; // 压缩后元素数量

    // Array size in bytes
    uint32_t originSizeBytes = 0;     // 原数组大小
    uint32_t compressedSizeBytes = 0; // 压缩后数组大小

    // Compression cost
    double compressTimeMs = 0;
    double decompressTimeMs = 0;

    // Compression ratio
    double compressionRatio = 0.0;
} CalResult;

// 接口定义
class SparseArrayCompressor
{
public:
    virtual ~SparseArrayCompressor() = default;

    // 压缩主入口
    virtual int8_t Compress(const ArrayInput &input) = 0; // TODO: const + 引用传递

    // 解压主入口
    virtual int8_t Decompress(ArrayInput &output) = 0;

    // 获取压缩结果
    virtual int8_t GetResult(CalResult &result) const = 0; //= 结果不一定只有一个，如一维与二维
};

// 工厂注册器
using CompressorFactory = std::function<std::unique_ptr<SparseArrayCompressor>()>; // TODO: 记录function用法

class CompressorRegistry
{
public:
    static CompressorRegistry &Instance(); // TODO: 单例模式， static成员函数

    void Register(const std::string &name, CompressorFactory factory);            // TODO: 注册保存function，与算法对象实现分离
    std::unique_ptr<SparseArrayCompressor> Create(const std::string &name) const; // TODO: 算法对象延迟创建
    std::vector<std::string> ListAlgorithms() const;

private:
    std::unordered_map<std::string, CompressorFactory> _factories;
};

#endif // _SPARSE_ARRAY_ANALYZER_H_