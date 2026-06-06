#pragma once

#include <array>
#include <filesystem>
#include <ostream>
#include <random>
#include <string>
#include <vector>

#include "Sorting.h"
#include "Timer.h"

namespace NBenchmark
{

struct SBenchmarkResult
{
    std::string caseName;
    std::string dataType;
    std::string algorithmName;
    std::size_t elementCount = 0;
    long long durationMicroseconds = 0;
    bool sorted = false;
};

class CBenchmarkSuite
{
public:
    std::vector<SBenchmarkResult> RunAll(std::ostream& output) const;

    static void SaveResultsToCsv(
        const std::filesystem::path& filePath,
        const std::vector<SBenchmarkResult>& results);

private:
    template<typename T>
    using TSortFunction = void(*)(std::vector<T>&);

    template<typename T>
    struct SAlgorithmEntry
    {
        const char* name;
        TSortFunction<T> function;
    };

    template<typename T, typename TGenerator>
    std::vector<SBenchmarkResult> RunCase(
        const std::string& caseName,
        const std::string& dataType,
        std::size_t elementCount,
        TGenerator generator) const
    {
        const std::array<SAlgorithmEntry<T>, 4> algorithms = {{
            {"BubbleSortClassic", &BubbleSortClassic<T>},
            {"BubbleSortOptimized", &BubbleSortOptimized<T>},
            {"InsertionSort", &InsertionSort<T>},
            {"std::sort", &StdSort<T>}
        }};

        const std::vector<T> sourceValues = generator(elementCount);
        std::vector<SBenchmarkResult> results;
        results.reserve(algorithms.size());

        for (const auto& algorithm : algorithms)
        {
            std::vector<T> values = sourceValues;
            CTimer timer;
            algorithm.function(values);

            results.push_back({
                caseName,
                dataType,
                algorithm.name,
                elementCount,
                timer.ElapsedMicroseconds(),
                IsSorted(values)
            });
        }

        return results;
    }

    static std::vector<int> GenerateRandomIntegers(std::size_t size, int minValue, int maxValue);
    static std::vector<int> GenerateSortedIntegers(std::size_t size);
    static std::vector<double> GenerateRandomDoubles(std::size_t size, double minValue, double maxValue);
    static std::vector<std::string> GenerateRandomStrings(
        std::size_t size,
        std::size_t minLength,
        std::size_t maxLength);
};

} // namespace NBenchmark
