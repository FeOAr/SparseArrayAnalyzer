#include "common.h"
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

std::vector<uint32_t> loadArrayFromTxt(const std::string &filename)
{
    std::vector<uint32_t> data;

    // 路径检查
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "Error: File does not exist: " << filename << std::endl;
        return data;
    }

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Failed to open file: " << filename << std::endl;
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

void printVector(const std::vector<uint32_t> &vec, size_t elemsPerLine)
{
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
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