/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-02 20:10:40
 * @FilePath: \SparseArrayAnalyzer\core\src\sparse_array_analyzer.cpp
 * @Description: 
 * 
 */

#include <stdio.h>
#include "sparse_array_analyzer.h"

void printfHelloWorld()
{
    printf("hello world\n");
}

CompressorRegistry &CompressorRegistry::Instance()
{
    static CompressorRegistry instance; // 本地静态变量，只会初始化一次
    return instance; // 返回它的引用
}
