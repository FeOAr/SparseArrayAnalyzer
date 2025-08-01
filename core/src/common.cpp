#include "common.h"
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <bitset>

std::vector<uint32_t> LoadArrayFromTxt(const std::string &filename)
{
    std::vector<uint32_t> data;

    // 路径检查
    if (!std::filesystem::exists(filename))
    {
        std::cerr << LOG_ERROR << "File does not exist: " << filename << std::endl;
        return data;
    }

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << LOG_ERROR << "Failed to open file: " << filename << std::endl;
        return data;
    }

    uint32_t value;
    while (file)
    {
        file >> value;
        if (file.fail())
        {
            if (file.eof())
                break;
            std::cerr << "Warning: Invalid data encountered. Skipping...\n";
            file.clear();                                                  // 清除错误状态
            file.ignore(std::numeric_limits<std::streamsize>::max(), ','); // 跳过无效片段
            continue;
        }
        data.push_back(value);

        // 处理逗号分隔
        if (file.peek() == ',')
            file.ignore();
    }

    file.close();
    return data;
}

uint32_t ParseInt(const char *str)
{
    try
    {
        size_t idx;
        uint32_t value = std::stoi(str, &idx);
        if (idx != std::strlen(str))
        {
            throw std::invalid_argument("Non-integer characters detected");
        }
        return value;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Invalid integer input: " << str << " (" << e.what() << ")\n";
        std::exit(EXIT_FAILURE);
    }
}

int8_t ReshapeTo2D(const std::vector<uint32_t> &input, const uint32_t row, const uint32_t col, std::vector<std::vector<uint32_t>> &output)
{
    if (input.empty())
    {
        std::cerr << LOG_ERROR << "Input data is empty.\n";
        return ERROR_INPUT_EMPTY;
    }

    if (row == 0 || col == 0)
    {
        std::cerr << LOG_ERROR << "Row or column size cannot be zero.\n";
        return ERROR_PARAM_INVALID;
    }

    if (row * col != input.size())
    {
        std::cerr << LOG_ERROR << "The specified dimensions is not compatible with the input data size.\n";
        return ERROR_PARAM_INVALID;
    }

    output.clear();
    output.resize(row, std::vector<uint32_t>(col, 0));

    size_t index = 0;
    for (uint32_t rowIdx = 0; rowIdx < row; ++rowIdx)
    {
        for (uint32_t colIdx = 0; colIdx < col; ++colIdx)
        {
            if (index < input.size())
            {
                output[rowIdx][colIdx] = input[index++];
            }
            else
            {
                output[rowIdx][colIdx] = 0; // 填充剩余部分为0
            }
        }
    }
    return SAA_SUCCESS;
}

void PrintBuffer(const uint8_t* data, size_t size, size_t perLine)
{
    for (size_t i = 0; i < size; ++i)
    {
        std::cout << "[" << std::setw(4) << std::setfill('0') << i << "]  "
                  << "0x" << std::hex << std::uppercase << std::setw(2) << static_cast<int>(data[i]) << std::dec
                  << "  =  " << std::bitset<8>(data[i]) << "\n";

        if ((i + 1) % perLine == 0)
            std::cout << std::endl;
    }
    std::cout << std::setfill(' ');  // TODO: 流填充恢复
}

void PrintBuffer(const std::vector<uint8_t>& vec, size_t perLine)
{
    PrintBuffer(vec.data(), vec.size(), perLine);
}

void PrintVector1D(const std::vector<uint32_t> &vec, const std::string commit, size_t elemsPerLine)
{
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Array commit: " << commit << std::endl;
    std::cout << "Array content (size = " << vec.size() << "):\n{\n    ";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << std::setw(8) << vec[i];
        if (i != vec.size() - 1)
        {
            std::cout << ",";
        }

        if ((i + 1) % elemsPerLine == 0 && i != vec.size() - 1)
        {
            std::cout << "\n    ";
        }
        else
        {
            std::cout << " ";
        }
    }
    std::cout << "\n}\n";
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
}

void PrintVector2D(const std::vector<std::vector<uint32_t>> &data, const uint32_t row, const uint32_t col, const std::string commit)
{
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Array commit: " << commit << std::endl;
    printf("2D Array content (size = %ux%u):\n{\n", row, col);
    size_t index = 0;
    for (uint32_t rowIdx = 0; rowIdx < row; ++rowIdx)
    {
        for (uint32_t colIdx = 0; colIdx < col; ++colIdx)
        {
            std::cout << std::setw(8) << data[rowIdx][colIdx];
            if (index != data.size() - 1)
            {
                std::cout << ",";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n}\n";
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
}

uint32_t GetArrayTotalSize1D(const std::vector<uint32_t> &data)
{
    uint32_t size = 0;
    size = GetArrayElemCount1D(data) * sizeof(uint32_t);
    return size;
}

uint32_t GetArrayTotalSize2D(const std::vector<std::vector<uint32_t>> &data)
{
    uint32_t size = 0;
    size = GetArrayElemCount2D(data) * sizeof(uint32_t);
    return size;
}

uint32_t GetArrayElemCount1D(const std::vector<uint32_t> &data)
{
    return static_cast<uint32_t>(data.size());
}

uint32_t GetArrayElemCount2D(const std::vector<std::vector<uint32_t>> &data)
{
    uint32_t count = 0;
    for (const auto &row : data)
    {
        count += static_cast<uint32_t>(row.size());
    }
    return count;
}

bool Compare2D(const std::vector<std::vector<uint32_t>> &a,
               const std::vector<std::vector<uint32_t>> &b)
{
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}
