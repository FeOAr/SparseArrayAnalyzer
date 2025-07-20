/*
 * @Author: FeOAr feoar@outlook.com
 * @Date: 2025-07-18 11:33:27
 * @LastEditors: FeOAr feoar@outlook.com
 * @LastEditTime: 2025-07-18 11:53:46
 * @FilePath: \SparseArrayAnalyzer\core\src\tool.hpp
 * @Description: 
 * 
 */
#pragma once
#include <vector>
#include <iostream>
#include <iomanip>
#include <type_traits>

template<typename T>
void PrintAnyVector1D(const std::vector<T>& vec, const std::string commit = "\" No commit.\"", size_t elemsPerLine = 8)
{
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Array commit: " << commit << std::endl;
    std::cout << "Array content (size = " << vec.size() << "):\n{\n    ";

    for (size_t i = 0; i < vec.size(); ++i)
    {
        if constexpr (std::is_same<T, uint8_t>::value)
        {
            // 避免 uint8_t 被解释为 char
            std::cout << std::setw(4) << static_cast<uint16_t>(vec[i]);
        }
        else
        {
            std::cout << std::setw(8) << vec[i];
        }

        if (i != vec.size() - 1)
            std::cout << ",";

        if ((i + 1) % elemsPerLine == 0 && i != vec.size() - 1)
            std::cout << "\n    ";
        else
            std::cout << " ";
    }

    std::cout << "\n}\n";
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;
}
