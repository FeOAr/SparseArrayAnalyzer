#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <getopt.h>
#include <cstdlib>

enum PatternType
{
    DIAGONAL,
    BANDED,
    BLOCK,
    INVALID
};

struct Options
{
    int rows = 0;
    int cols = 0;
    double sparsity = 0.9;
    int minValue = 1;
    int maxValue = 100;
    int mainValue = 0;
    std::string output = "matrix.txt";
    PatternType pattern = INVALID;
};

PatternType parsePattern(const std::string &str)
{
    if (str == "diagonal")
        return DIAGONAL;
    if (str == "banded")
        return BANDED;
    if (str == "block")
        return BLOCK;
    return INVALID;
}

void writeMatrix(const std::vector<std::vector<int>> &mat, const std::string &filename)
{
    std::ofstream ofs(filename);
    for (const auto &row : mat)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            ofs << row[i];
            if (i < row.size() - 1)
                ofs << " ";
        }
        ofs << "\n";
    }
}

void generateDiagonal(Options opts, std::vector<std::vector<int>> &mat, std::mt19937 &rng, std::uniform_int_distribution<> &dist)
{
    int diag_len = std::min(opts.rows, opts.cols);
    for (int i = 0; i < opts.rows; ++i)
    {
        for (int j = 0; j < opts.cols; ++j)
        {
            if (i == j && i < diag_len)
            {
                mat[i][j] = dist(rng);
            }
            else
            {
                mat[i][j] = opts.mainValue;
            }
        }
    }
}

void generateBanded(Options opts, std::vector<std::vector<int>> &mat, std::mt19937 &rng, std::uniform_int_distribution<> &dist)
{
    int bandwidth = static_cast<int>((1.0f - opts.sparsity) * opts.cols);
    for (int i = 0; i < opts.rows; ++i)
    {
        for (int j = 0; j < opts.cols; ++j)
        {
            if (std::abs(i - j) <= bandwidth / 2)
            {
                mat[i][j] = dist(rng);
            }
            else
            {
                mat[i][j] = opts.mainValue;
            }
        }
    }
}

void generateBlock(Options opts, std::vector<std::vector<int>> &mat, std::mt19937 &rng, std::uniform_int_distribution<> &dist)
{
    int block_rows = 10;
    int block_cols = 10;
    for (int i = 0; i < opts.rows; ++i)
    {
        for (int j = 0; j < opts.cols; ++j)
        {
            if ((i / block_rows + j / block_cols) % 3 == 0)
            {
                mat[i][j] = dist(rng);
            }
            else
            {
                mat[i][j] = opts.mainValue;
            }
        }
    }
}

void PrintUsageGuide()
{
    std::cerr << R"(Usage:
  generate_sparse_matrix -r <rows> -c <cols> -p <pattern> -s <sparsity> -m <min> -n <max> -o <output>

Options:
  -r <rows>         Number of rows (positive integer)
  -c <cols>         Number of columns (positive integer)
  -p <pattern>      Pattern type: diagonal | banded | block
  -s <sparsity>     Sparsity (0.0 ~ 1.0, exclusive)
  -m <min>          Minimum value of non-zero elements
  -n <max>          Maximum value of non-zero elements
  -o <output>       Output file name
  -v <mainValue>    Main fill value (default = 0)

Example:
  generate_sparse_matrix -r 1000 -c 1000 -p diagonal -s 0.95 -m 1 -n 100 -o out.txt
)";
}

int main(int argc, char **argv)
{
    Options opts;

    int opt;
    while ((opt = getopt(argc, argv, "r:c:p:s:m:n:o:v:")) != -1)
    {
        switch (opt)
        {
        case 'r':
            opts.rows = std::atoi(optarg);
            break;
        case 'c':
            opts.cols = std::atoi(optarg);
            break;
        case 'p':
            opts.pattern = parsePattern(optarg);
            break;
        case 's':
            opts.sparsity = std::atof(optarg);
            break;
        case 'm':
            opts.minValue = std::atoi(optarg);
            break;
        case 'n':
            opts.maxValue = std::atoi(optarg);
            break;
        case 'o':
            opts.output = optarg;
            break;
        case 'v':
            opts.mainValue = std::atoi(optarg);
            break;
        default:
            PrintUsageGuide();
            return 1;
        }
    }

    // 参数完整性检查
    if (opts.rows <= 0 || opts.cols <= 0)
    {
        std::cerr << "Error: Rows and columns must be positive integers.\n\n";
        PrintUsageGuide();
        return 1;
    }

    if (opts.sparsity < 0.0 || opts.sparsity >= 1.0)
    {
        std::cerr << "Error: Sparsity must be in the range (0.0, 1.0).\n\n";
        PrintUsageGuide();
        return 1;
    }

    if (opts.minValue > opts.maxValue)
    {
        std::cerr << "Error: minValue cannot be greater than maxValue.\n\n";
        PrintUsageGuide();
        return 1;
    }

    if (opts.pattern == INVALID)
    {
        std::cerr << "Error: Unsupported pattern type.\n\n";
        PrintUsageGuide();
        return 1;
    }

    // 构造稀疏矩阵并生成
    std::vector<std::vector<int>> matrix(opts.rows, std::vector<int>(opts.cols, opts.mainValue));
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dist(opts.minValue, opts.maxValue);

    switch (opts.pattern)
    {
    case DIAGONAL:
        generateDiagonal(opts, matrix, rng, dist);
        break;
    case BANDED:
        generateBanded(opts, matrix, rng, dist);
        break;
    case BLOCK:
        generateBlock(opts, matrix, rng, dist);
        break;
    default:
        break;
    }

    writeMatrix(matrix, opts.output);
    std::cout << "Matrix saved to " << opts.output << std::endl;
    return 0;
}
