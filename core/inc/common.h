#ifndef _COMMON_H_
#define _COMMON_H_
#include <iostream>
#include <vector>
#include <cstdint>

std::vector<uint32_t> loadArrayFromTxt(const std::string& filename);
void printVector(const std::vector<uint32_t>& data, size_t elemsPerLine = 16);

#endif // _COMMON_H_