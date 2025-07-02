/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-06-30 20:46:31
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-06-30 21:23:29
 * @FilePath: \SparseArrayAnalyzer\test\main.cpp
 * @Description:
 *
 */
#include "sparse_array_analyzer.h"
#include <iostream>
#include <memory>
#include <vector>
#include "common.h"

int main()
{
    printf("Start analyze!\n");
    
    // 原始数组
    std::vector<uint32_t> data = loadArrayFromTxt("test\\input_array.txt");

    printVector(data);

    return 0;
}
