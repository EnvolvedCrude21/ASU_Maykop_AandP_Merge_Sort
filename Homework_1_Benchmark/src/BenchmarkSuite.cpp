#include "BenchmarkSuite.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace NBenchmark
{

namespace
{

void PrintCaseResults(
    std::ostream& output,
    const std::string& caseTitle,
    const std::vector<SBenchmarkResult>& results)
{
    output << "\n=== " << caseTitle << " ===\n";
    for (const auto& result : results)
    {
        output
            << std::left << std::setw(20) << result.algorithmName
            << " | " << std::setw(8) << result.dataType
            << " | size = " << std::setw(5) << result.elementCount
            << " | " << std::setw(8) << result.durationMicroseconds << " us"
            << " | " << (result.sorted ? "OK" : "FAIL")
            << '\n';
    }
}

void AppendResults(
    std::vector<SBenchmarkResult>& destination,
    const std::vector<SBenchmarkResult>& source)
{
    destination.insert(destination.end(), source.begin(), source.end());
}

} // namespace

std::vector<SBenchmarkResult> CBenchmarkSuite::RunAll(std::ostream& output) const
{
    std::vector<SBenchmarkResult> allResults;

    const auto intRandomResults = RunCase<int>(
        "Random integers",
        "int",
        1000,
        [](std::size_t size)
        {
            return GenerateRandomIntegers(size, -5000, 5000);
        });
    PrintCaseResults(output, "Random integers", intRandomResults);
    AppendResults(allResults, intRandomResults);

    const auto intSortedResults = RunCase<int>(
        "Sorted integers",
        "int",
        2000,
        [](std::size_t size)
        {
            return GenerateSortedIntegers(size);
        });
    PrintCaseResults(output, "Sorted integers", intSortedResults);
    AppendResults(allResults, intSortedResults);

    const auto doubleRandomResults = RunCase<double>(
        "Random doubles",
        "double",
        1500,
        [](std::size_t size)
        {
            return GenerateRandomDoubles(size, -1000.0, 1000.0);
        });
    PrintCaseResults(output, "Random doubles", doubleRandomResults);
    AppendResults(allResults, doubleRandomResults);

    const auto stringRandomResults = RunCase<std::string>(
        "Random strings",
        "string",
        350,
        [](std::size_t size)
        {
            return GenerateRandomStrings(size, 5, 15);
        });
    PrintCaseResults(output, "Random strings", stringRandomResults);
    AppendResults(allResults, stringRandomResults);

    output << "\nTotal results: " << allResults.size() << '\n';
    return allResults;
}

void CBenchmarkSuite::SaveResultsToCsv(
    const std::filesystem::path& filePath,
    const std::vector<SBenchmarkResult>& results)
{
    std::ofstream file(filePath);
    if (!file)
    {
        throw std::runtime_error("Unable to open the results file for writing.");
    }

    file << "case_name,data_type,algorithm,size,duration_us,sorted\n";
    for (const auto& result : results)
    {
        file
            << result.caseName << ','
            << result.dataType << ','
            << result.algorithmName << ','
            << result.elementCount << ','
            << result.durationMicroseconds << ','
            << (result.sorted ? "true" : "false")
            << '\n';
    }
}

std::vector<int> CBenchmarkSuite::GenerateRandomIntegers(
    std::size_t size,
    int minValue,
    int maxValue)
{
    std::mt19937 generator(42u + static_cast<unsigned int>(size));
    std::uniform_int_distribution<int> distribution(minValue, maxValue);

    std::vector<int> values(size);
    for (auto& value : values)
    {
        value = distribution(generator);
    }

    return values;
}

std::vector<int> CBenchmarkSuite::GenerateSortedIntegers(std::size_t size)
{
    std::vector<int> values(size);
    for (std::size_t index = 0; index < size; ++index)
    {
        values[index] = static_cast<int>(index);
    }

    return values;
}

std::vector<double> CBenchmarkSuite::GenerateRandomDoubles(
    std::size_t size,
    double minValue,
    double maxValue)
{
    std::mt19937 generator(1337u + static_cast<unsigned int>(size));
    std::uniform_real_distribution<double> distribution(minValue, maxValue);

    std::vector<double> values(size);
    for (auto& value : values)
    {
        value = distribution(generator);
    }

    return values;
}

std::vector<std::string> CBenchmarkSuite::GenerateRandomStrings(
    std::size_t size,
    std::size_t minLength,
    std::size_t maxLength)
{
    std::mt19937 generator(2026u + static_cast<unsigned int>(size));
    std::uniform_int_distribution<int> lengthDistribution(
        static_cast<int>(minLength),
        static_cast<int>(maxLength));
    std::uniform_int_distribution<int> characterDistribution('a', 'z');

    std::vector<std::string> values(size);
    for (auto& value : values)
    {
        const auto length = static_cast<std::size_t>(lengthDistribution(generator));
        value.resize(length);

        for (char& symbol : value)
        {
            symbol = static_cast<char>(characterDistribution(generator));
        }
    }

    return values;
}

} // namespace NBenchmark
