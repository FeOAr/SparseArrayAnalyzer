#ifndef _COMMON_H_
#define _COMMON_H_
#include <iostream>
#include <vector>
#include <cstdint>

#define SAA_SUCCESS (0)
#define ERROR_INPUT_EMPTY (-1)
#define ERROR_PARAM_INVALID (-2)

std::vector<uint32_t> loadArrayFromTxt(const std::string& filename);
uint32_t parseInt(const char* str);
int8_t reshapeTo2D(const std::vector<uint32_t>& input, const uint32_t row, const uint32_t col, std::vector<std::vector<uint32_t>>& output);

void printVector1D(const std::vector<uint32_t>& data, size_t elemsPerLine = 16);
void printVector2D(const std::vector<std::vector<uint32_t>>& data, const uint32_t row, const uint32_t col);

#endif // _COMMON_H_