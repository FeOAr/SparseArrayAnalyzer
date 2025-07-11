/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-02 19:33:17
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-09 21:07:54
 * @FilePath: \SparseArrayAnalyzer\core\inc\common.h
 * @Description: 
 * 
 */
#ifndef _COMMON_H_
#define _COMMON_H_
#include <iostream>
#include <vector>
#include <cstdint>

#define SAA_SUCCESS (0)
#define ERROR_INPUT_EMPTY (-1)
#define ERROR_PARAM_INVALID (-2)

std::vector<uint32_t> LoadArrayFromTxt(const std::string& filename);
uint32_t ParseInt(const char* str);
int8_t ReshapeTo2D(const std::vector<uint32_t>& input, const uint32_t row, const uint32_t col, std::vector<std::vector<uint32_t>>& output);

void PrintVector1D(const std::vector<uint32_t>& data, size_t elemsPerLine = 16);
void PrintVector2D(const std::vector<std::vector<uint32_t>>& data, const uint32_t row, const uint32_t col);

uint32_t GetArrayTotalSize1D(const std::vector<uint32_t> &data);
uint32_t GetArrayTotalSize2D(const std::vector<std::vector<uint32_t>> &data);

uint32_t GetArrayElemCount1D(const std::vector<uint32_t> &data);
uint32_t GetArrayElemCount2D(const std::vector<std::vector<uint32_t>> &data);

#endif // _COMMON_H_