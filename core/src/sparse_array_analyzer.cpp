/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-06 15:30:58
 * @FilePath: \SparseArrayAnalyzer\core\src\sparse_array_analyzer.cpp
 * @Description:
 *
 */

#include <stdio.h>
#include "sparse_array_analyzer.h"

CompressorRegistry &CompressorRegistry::Instance()
{
    static CompressorRegistry instance; // 本地静态变量，只会初始化一次
    return instance;                    // 返回它的引用
}

void CompressorRegistry::Register(const std::string &name, CompressorFactory factory)
{
    _factories[name] = std::move(factory);  //TODO: 学习这种操作方式
}

std::unique_ptr<SparseArrayCompressor> CompressorRegistry::Create(const std::string &name) const
{
    auto it = _factories.find(name);
    if (it != _factories.end())
    {
        return it->second(); // 调用工厂函数返回实例
    }
    else
    {
        return nullptr;
    }
}

// 列出所有注册的算法名称
std::vector<std::string> CompressorRegistry::ListAlgorithms() const
{
    std::vector<std::string> names;
    for (const auto &pair : _factories)
    {
        names.push_back(pair.first);
    }
    return names;
}