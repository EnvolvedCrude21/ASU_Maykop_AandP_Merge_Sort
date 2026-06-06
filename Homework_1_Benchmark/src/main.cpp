#include <filesystem>
#include <iostream>

#include "BenchmarkSuite.h"

int main()
{
    try
    {
        NBenchmark::CBenchmarkSuite benchmarkSuite;
        const auto results = benchmarkSuite.RunAll(std::cout);

        const auto resultFilePath = std::filesystem::current_path() / "benchmark_results.csv";
        NBenchmark::CBenchmarkSuite::SaveResultsToCsv(resultFilePath, results);

        std::cout << "Results saved to benchmark_results.csv\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}
