#include <algorithm>
#include <filesystem>
#include <iostream>

#include "BenchmarkSuite.h"

int main()
{
    try
    {
        std::cout << "Running benchmark tests\n";

        NBenchmark::CBenchmarkSuite benchmarkSuite;
        const auto results = benchmarkSuite.RunAll(std::cout);

        const auto failedResults = static_cast<int>(std::count_if(
            results.begin(),
            results.end(),
            [](const auto& result)
            {
                return !result.sorted;
            }));

        const auto resultFilePath = std::filesystem::current_path() / "benchmark_test_results.csv";
        NBenchmark::CBenchmarkSuite::SaveResultsToCsv(resultFilePath, results);

        std::cout << "\nSummary: " << (results.size() - failedResults) << " successful checks, "
                  << failedResults << " failed\n";
        std::cout << "Results file: benchmark_test_results.csv\n";

        return failedResults == 0 ? 0 : 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Error: " << exception.what() << '\n';
        return 1;
    }
}
