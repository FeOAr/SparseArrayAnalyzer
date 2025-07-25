/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-02 19:33:17
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-19 23:38:39
 * @FilePath: \SparseArrayAnalyzer\core\inc\common.h
 * @Description: 
 * 
 */
#ifndef _COMMON_H_
#define _COMMON_H_
#include <iostream>
#include <vector>
#include <cstdint>
#include <string>

/* ---------------------------------- Color --------------------------------- */
#define COLOR_RED    "\033[0;30;41m"
#define COLOR_GREEN  "\033[0;30;42m"
#define COLOR_YELLOW "\033[0;30;43m"
#define COLOR_BLUE   "\033[0;30;44m"
#define COLOR_PURPLE "\033[0;30;45m"
#define COLOR_WHITE  "\033[0;30;47m"
#define COLOR_RESET  "\033[0m "

#define COLOR_STR(str, color) (std::string(color) + str + COLOR_RESET)
#define LOG_ERROR COLOR_RED    << "[ERROR]" << COLOR_RESET << __FILE__ << ":" << __LINE__ << " -> "
#define LOG_WARN  COLOR_YELLOW << "[WARN]"  << COLOR_RESET << __FILE__ << ":" << __LINE__ << " -> "
#define LOG_INFO  COLOR_GREEN  << "[INFO]"  << COLOR_RESET << __FILE__ << ":" << __LINE__ << " -> "
#define LOG_DEBUG COLOR_WHITE  << "[DEBUG]" << COLOR_RESET << __FILE__ << ":" << __LINE__ << " -> "

/* ------------------------------- Error code ------------------------------- */
#define SAA_SUCCESS               (0)
#define ERROR_INPUT_EMPTY         (-1)
#define ERROR_PARAM_INVALID       (-2)
#define ERROR_UNSUPPORT_DIMENSION (-3)
#define ERROR_CALCULATE_ERROR     (-4)
#define ERROR_UNKNOW_ERROR        (-5)
#define ERROR_INDEX_OUT_OF_RANGE  (-6)

/* ---------------------------- Algorithm enable ---------------------------- */
#define ENABLE                    (1)
#define BYPASS                    (0)

#define ALGORITHM_DENSE           (ENABLE)
#define ALGORITHM_COORDINATE      (ENABLE)
#define ALGORITHM_RUN_LENGTH      (BYPASS)
#define ALGORITHM_BITMAP_PAYLOAD  (ENABLE)
#define ALGORITHM_DICTIONARY      (ENABLE)
#define ALGORITHM_CSR             (ENABLE)
#define ALGORITHM_CSC             (ENABLE)

/* -------------------------------- function -------------------------------- */
typedef enum array_dimension
{
    ARRAY_1D = 0, // 一维数组
    ARRAY_2D = 1, // 二维数组
} ArrayDimension;

std::vector<uint32_t> LoadArrayFromTxt(const std::string& filename);
uint32_t ParseInt(const char* str);
int8_t ReshapeTo2D(const std::vector<uint32_t>& input, const uint32_t row, const uint32_t col, std::vector<std::vector<uint32_t>>& output);

void PrintBuffer(const std::vector<uint8_t>& vec, size_t perLine = 8);
void PrintVector1D(const std::vector<uint32_t>& data, const std::string commit = "\" No commit.\"", size_t elemsPerLine = 16);
void PrintVector2D(const std::vector<std::vector<uint32_t>>& data, const uint32_t row, const uint32_t col, const std::string commit = "\" No commit.\"");

uint32_t GetArrayTotalSize1D(const std::vector<uint32_t> &data);
uint32_t GetArrayTotalSize2D(const std::vector<std::vector<uint32_t>> &data);

uint32_t GetArrayElemCount1D(const std::vector<uint32_t> &data);
uint32_t GetArrayElemCount2D(const std::vector<std::vector<uint32_t>> &data);

bool Compare2D(const std::vector<std::vector<uint32_t>> &a, const std::vector<std::vector<uint32_t>> &b);

#endif // _COMMON_H_